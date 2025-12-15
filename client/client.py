import socket
import json
import sys
import logging
from assistant import Assistant
from furhat_realtime_api import FurhatClient

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080


#TO DO : FAI IN MODO CHE OGNI MESSAGGIO PASSI DALL'IA PRIMA DI ESSERE PARLATO DA FURHAT
#GESTISCI LE DOMANDE DEL TIPI IN MODO DA FARLE UNA ALLA VOLTA E ASPETTARE LA RISPOSTA PRIMA DI ANDARE AVANTI

def main():
    try:
        furhat = connectToFurhat()
        
        request = askLanguage(furhat)
        response = connectAndSend(request)
        request = askTipi(furhat, response)
        response = connectAndSend(request)

        assistant = Assistant(response.get("config", {}))

        while True:
            userInput = furhat.request_listen_start()
            assistantResponse = assistant.getResponse(userInput)
            textToSpeak = assistantResponse.get("text", "I didn't understand that.")
            gestureToPerform = assistantResponse.get("gesture", None)
            if assistantResponse.get("end_conversation") == True:
                furhat.request_speak_text("Thank you for your time. Goodbye!")
                break
            else:
                furhat.request_speak_text(textToSpeak)
                if gestureToPerform:
                    furhat.request_gesture(gestureToPerform)


    except Exception as e:
        print(f"Errore: {e}")
    finally:
        furhatShutdown(furhat)
        print("Connessione chiusa.")

def furhatShutdown(furhat):
    if furhat:
        furhat.disconnect()


def connectAndSend(request):
    """
    Connects to the server, sends a JSON request, and returns the JSON response.
    """
    # Create the socket using a context manager to ensure it closes automatically
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        
        # Serialize and send
        client_socket.sendall(json.dumps(request).encode('utf-8'))

        # Receive and deserialize
        # Note: This assumes the response fits in 4096 bytes
        response_data = client_socket.recv(4096)
        
        return json.loads(response_data.decode('utf-8'))

def connectToFurhat():
    furhat = FurhatClient("127.0.0.1")
    furhat.set_logging_level(logging.INFO)
    furhat.connect()
    furhat.request_speak_text("Hello world, I am Furhat.")
    return furhat


#TO DO : IMPLEMENTARE IL RICONOSCIMENTO DELLA LINGUA DA PARTE DI FURHAT
def askLanguage(furhat):
    furhat.request_speak_text("Which language would you like to speak?")
    #language = furhat.listen_for_language_choice()
    language = "english"  # for testing purposes
    request = {}
    if language == "italian":
        request = { "type": 'set_language', "language": "it" }
    if language == "english":
        request = { "type": 'set_language', "language": "en" }
    return request

def performTipiTest(furhat, response):
    scores = []
    furhat.say("I need to ask you 10 questions. Please answer with a number from 1 to 7.")
    
    for i, question in enumerate(response.get("questions", [])):
        furhat.say(f"Question {i+1}. {question}")
        
        # PER ORA: Usiamo input tastiera per testare velocemente la logica
        # (Il riconoscimento vocale dei numeri è lento e prono a errori senza grammatiche)
        while True:
            try:
                val = int(input(f"Voto per '{question}' (1-7): "))
                if 1 <= val <= 7:
                    scores.append(val)
                    furhat.gesture(name="Blink") # Feedback visivo
                    break
                print("Inserisci un numero tra 1 e 7.")
            except ValueError:
                print("Numero non valido.")
                
    return scores


if __name__ == "__main__":
    main()
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

        request = getQuestions()
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

def getQuestions():
    return {"type": "get_questions"}

def askTipi(furhat, response):
    questions = response.get("questions", [])
    scores = []
    furhat.request_speak_text(f"I have {len(questions)} questions. You can answer naturally.")

    for i, q_text in enumerate(questions):
        # Continua a chiedere la stessa domanda finché non ottiene una risposta valida
        while True:
            # Domanda
            furhat.request_speak_text(f"Question {i+1}. {q_text}")
            
            # Ascolto
            user_audio_text = furhat.request_listen_start()
            print(f"Input ricevuto per domanda {i+1}: {user_audio_text}")
            
            if not user_audio_text:
                furhat.request_speak_text("I didn't hear you.")
                continue

            # Parsing con AI (converte testo -> numero)
            score = parser_ai.parse_tipi_score(str(user_audio_text), q_text)
            
            if score is not None and 1 <= score <= 7:
                scores.append(score)
                # Feedback (Gesto rapido per confermare)
                furhat.request_gesture("Blink") 
                break
            else:
                furhat.request_speak_text("I didn't understand. Please say a number or answer yes or no.")

    return { "type": "tipi_submission", "scores": scores }

if __name__ == "__main__":
    main()
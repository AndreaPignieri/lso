import socket
import json
import sys
import logging
from assistant import Assistant
from furhat_realtime_api import FurhatClient

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080


def main():
    try:
        furhat = connectToFurhat()

        request = getQuestions()
        response = connectAndSend(request)
        request = askTipi(furhat, response)
        response = connectAndSend(request)

        assistant = Assistant(mode="persona", personality=response.get("config", {}))

        while True:
            userInput = furhat.request_listen_start()
            
            if not userInput:
                continue

            assistantResponse = assistant.getResponse(userInput)
            
            textToSpeak = assistantResponse.get("text", "Non ho capito bene, puoi ripetere?")
            gestureToPerform = assistantResponse.get("gesture", None)
            
            if assistantResponse.get("end_conversation") == True:
                furhat.request_speak_text("Grazie per la conversazione. Arrivederci!")
                break
            else:
                furhat.request_speak_text(textToSpeak)
                if gestureToPerform and gestureToPerform != "None":
                    try:
                        furhat.request_gesture(gestureToPerform)
                    except Exception as e:
                        print(f"Gesto fallito: {e}")


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
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        client_socket.sendall(json.dumps(request).encode('utf-8'))
        response = client_socket.recv(4096)
        
        if not response:
            raise Exception("Nessuna risposta dal server.")
        
        if response.get("status") == "error":
            raise Exception(f"Errore dal server: {response.get('message', 'Nessun messaggio di errore fornito')}")

        return json.loads(response.decode('utf-8'))

def connectToFurhat():
    furhat = FurhatClient("127.0.0.1")
    furhat.request_voice_config(language="it")
    furhat.set_logging_level(logging.INFO)
    furhat.connect()
    return furhat

def getQuestions():
    return {"type": "get_questions"}

def askTipi(furhat, response):
    questions = response.get("questions", [])
    scores = []
    furhat.request_speak_text(response.get("message"))
    parser_ai = Assistant(mode="parser")

    for i, q_text in enumerate(questions):
        while True:
            furhat.request_speak_text(f"Question {i+1}. {q_text}")
            user_audio_text = furhat.request_listen_start()
            print(f"Input ricevuto per domanda {i+1}: {user_audio_text}")
            
            if not user_audio_text:
                furhat.request_speak_text("Non ho sentito nulla. Per favore ripeti la tua risposta.")
                continue
            
            score = parser_ai.parse_tipi_score(str(user_audio_text), q_text)
            
            if score is not None and 1 <= score <= 7:
                scores.append(score)
                furhat.request_gesture("Blink") 
                break
            else:
                furhat.request_speak_text("Non ho capito la tua risposta. Per favore rispondi con un numero da 1 a 7.")

    return { "type": "tipi_submission", "scores": scores }

if __name__ == "__main__":
    main()
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
        clientSocket = connectToSocket()
        furhat = connectToFurhat()

        print(f"Connesso al server {SERVER_IP}:{SERVER_PORT}")

        request = askLanguage(furhat)
        response = sendAndReceive(clientSocket, request)

        request = askTipi(furhat)
        response = sendAndReceive(clientSocket, request)

        assistant = Assistant(response.get("config", {}))

        while True:
            userInput = furhat.listen_for_user_input()
            assistantResponse = assistant.getResponse(userInput)
            if assistantResponse.contains("end_conversation"):
                furhat.request_speak_text("Thank you for your time. Goodbye!")
                break
            else:
                furhat.request_speak_text(assistantResponse)


    except Exception as e:
        print(f"Errore: {e}")
    finally:
        clientSocket.close()
        print("Connessione chiusa.")

def connectToSocket():
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientSocket.connect((SERVER_IP, SERVER_PORT))
    return clientSocket

def connectToFurhat():
    furhat = FurhatClient("127.0.0.1")
    furhat.set_logging_level(logging.INFO)
    furhat.connect()
    furhat.request_speak_text("Hello world, I am Furhat.")
    return furhat

def askLanguage(furhat):
    furhat.request_speak_text("Which language would you like to speak?")
    #language = furhat.listen_for_language_choice()
    language = "italian"  # for testing purposes
    # listen to the input, send it to an AI model to detect language
    # for testing purposes, we will return italian
    if language == "italian":
        request = { "type": 'set_language', "language": "it" }
    if language == "english":
        request = { "type": 'set_language', "language": "en" }
    return request

def askTipi(furhat):
    furhat.request_speak_text("Please describe the tipi you would like to build.")
    #tipi_description = furhat.listen_for_tipi_description()
    tipi_description = "A large tipi with red and blue patterns."  # for testing purposes
    request = { "type": 'tipi_submission', "description": tipi_description }
    return request

def sendAndReceive(clientSocket, request):
    request_str = json.dumps(request)
    clientSocket.sendall(request_str.encode('utf-8'))

    response_data = clientSocket.recv(4096)
    response_str = response_data.decode('utf-8')
    response = json.loads(response_str)
    return response

if __name__ == "__main__":
    main()
import socket
import json
import os
import logging
from assistant import Assistant
from furhat_realtime_api import FurhatClient

FURHAT_IP = os.getenv("FURHAT_IP", "host.docker.internal")
SERVER_IP = os.getenv("SERVER_IP", "server_c")
SERVER_PORT = int(os.getenv("SERVER_PORT", 8080))

def main():
    furhat = None
    try:
        print(f"--- Tentativo di connessione a Furhat su {FURHAT_IP} ---")
        furhat = connectToFurhat()
        print("--- Connessione a Furhat riuscita! ---")

        goodMorning(furhat)

        print("--- Richiesta domande al server locale... ---")
        request = getQuestions()
        response = connectAndSend(request)
        
        print("--- Inizio intervista TIPI ---")
        request = askTipi(furhat, response)
        
        print("--- Invio risultati al server... ---")
        response = connectAndSend(request)

        print("--- Avvio Assistente ---")
        assistant = Assistant(mode="persona", personality=response.get("config", {}))
        furhat.request_speak_text(response.get("toSpeak", "Iniziamo la conversazione.  C'è qualcosa di cui vorresti parlare?"))

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
                        furhat.request_gesture_start(gestureToPerform)
                    except Exception as e:
                        print(f"Gesto fallito: {e}")

    except Exception as e:
        print(f"Dettaglio errore: {e}")
        furhat.request_speak_text("Mi dispiace, si è verificato un errore. Termino il programma.")
    finally:
        furhatShutdown(furhat)
        print("Programma terminato.")

def furhatShutdown(furhat):
    """Chiude la connessione con il robot in modo sicuro."""
    if furhat:
        try:
            furhat.request_speak_text("Sto chiudendo la connessione. A presto!")
            furhat.disconnect()
            print("Disconnessione da Furhat completata.")
        except:
            pass

def goodMorning(furhat):
    furhat.request_speak_text("Ciao! Sono Furhat, il tuo assistente virtuale, sei pronto per iniziare?")
    response = furhat.request_listen_start()
    if response:
        furhat.request_speak_text("Perfetto! Iniziamo la conversazione.")

def connectAndSend(request):
    """Gestisce la comunicazione socket con il server Python locale."""
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.settimeout(5) 
            client_socket.connect((SERVER_IP, SERVER_PORT))
            
            client_socket.sendall(json.dumps(request).encode('utf-8'))
            response_bytes = client_socket.recv(4096)
            
            if not response_bytes:
                raise Exception("Il server ha risposto con dati vuoti.")
            response_str = response_bytes.decode('utf-8')
            response_json = json.loads(response_str)
            if response_json.get("status") == "error":
                raise Exception(f"Errore dal server: {response_json.get('message', 'Nessun messaggio')}")
            return response_json
    except ConnectionRefusedError:
        raise Exception(f"Impossibile connettersi al server locale su {SERVER_IP}:{SERVER_PORT}. Assicurati che sia avviato.")
    except json.JSONDecodeError:
        raise Exception("Il server ha risposto, ma non era un JSON valido.")

def connectToFurhat():
    """Configura e connette il client Furhat."""
    furhat = FurhatClient(FURHAT_IP)
    furhat.set_logging_level(logging.INFO)
    furhat.connect()
    furhat.request_voice_config(voice_id="DiegoNeural (it-IT) - Microsoft Azure")
    furhat.request_attend_user("closest")
    return furhat

def getQuestions():
    """Crea il payload per richiedere le domande al server."""
    return {"type": "get_questions"}

def askTipi(furhat, response):
    """Gestisce il ciclo di domande e risposte del test TIPI."""
    questions = response.get("questions", [])
    scores = []
    
    message = response.get("message", "Iniziamo il test.")
    furhat.request_speak_text(message)

    parser_ai = Assistant(mode="parser")

    for i, q_text in enumerate(questions):
        while True:
            furhat.request_speak_text(f"Domanda numero {i+1}. {q_text}")
            user_audio_text = furhat.request_listen_start()
            print(f"Input ricevuto per domanda {i+1}: {user_audio_text}")

            if not user_audio_text:
                furhat.request_speak_text("Non ho sentito nulla. Per favore ripeti.")
                continue

            score = parser_ai.parse_tipi_score(str(user_audio_text), q_text)
            
            if score is not None and 1 <= score <= 7:
                scores.append(score)
                furhat.request_gesture_start("Blink") 
                break
            else:
                furhat.request_speak_text("Non ho capito. Rispondi con un numero da 1 a 7.")

    return { "type": "tipi_submission", "scores": scores }

if __name__ == "__main__":
    main()
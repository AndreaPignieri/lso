import json
from google import genai
from google.genai import types


class Assistant:
    def __init__(self, mode, personality=None):
        self.client = genai.Client()
        
        self.gestures = [
            "Smile", "BigSmile", "Wink", "Thoughtful", "Surprise", 
            "Oh", "BrowFrown", "BrowRaise", "Blink", "Nod", 
            "Shake", "Roll", "GazeAway", "ExpressDisgust", 
            "ExpressSad", "ExpressAnger", "ExpressFear"
        ]

        if mode == "persona":
            technical_instruction = f"""
                ISTRUZIONI TECNICHE:
                Stai controllando una testa robotica fisica chiamata Furhat.
                
                GESTI DISPONIBILI:
                {', '.join(self.gestures)}

                ISTRUZIONI:
                1. Rispondi SEMPRE in formato JSON valido.
                2. Decidi quale gesto si adatta meglio al tuo testo e all'umore della conversazione fornita nel contesto. Usa "Nod" per essere d'accordo, "Shake" per disaccordo, "Smile" per i saluti, ecc.
                3. Se non serve nessun gesto, usa null o "None".
                4. "text": La frase che vuoi pronunciare (in italiano).
                5. "gesture": Il nome del gesto da eseguire, o null se non serve.
                6. "end_conversation": Booleano, true solo se l'utente vuole smettere di parlare o saluta per andarsene.

                Esempio di output JSON:
                {{
                    "text": "Ciao! È un piacere conoscerti.",
                    "gesture": "BigSmile",
                    "end_conversation": false
                }}
            """
            system_prompt = f"{personality}\n\n{technical_instruction}"
            
            self.chat = self.client.chats.create(
                model='gemini-2.5-flash',
                config=types.GenerateContentConfig(
                    system_instruction=system_prompt,
                    response_mime_type="application/json",
                    temperature=0.7
                )
            )
        else:
            system_prompt = """
            RUOLO: Sei un rigoroso parser di dati.
            COMPITO: Converti la risposta in linguaggio naturale dell'utente in un punteggio TIPI (intero da 1 a 7).
            SCALA:
            1 = Fortemente in disaccordo
            2 = Moderatamente in disaccordo
            3 = Leggermente in disaccordo
            4 = Né d'accordo né in disaccordo
            5 = Leggermente d'accordo
            6 = Moderatamente d'accordo
            7 = Fortemente d'accordo
            
            FORMATO OUTPUT: Solo JSON.
            Esempio: {"score": 7} oppure {"score": null} se l'input non è valido o non c'entra nulla.
            NON generare alcun testo aggiuntivo, solo il JSON.
            """
            self.chat = self.client.chats.create(
            model='gemini-2.5-flash',
            config=types.GenerateContentConfig(
                system_instruction=system_prompt,
                response_mime_type="application/json",
                temperature=0.1
                )
            )

    def getResponse(self, user_input):
        try:
            response = self.chat.send_message(user_input)
            response_data = json.loads(response.text)
            return response_data
        except Exception as e:
            print(f"Errore AI: {e}")
            return {"text": "Ho qualche problema a pensare in questo momento.", "gesture": "BrowFrown", "end_conversation": False}
        
    def parse_tipi_score(self, user_answer, current_question):
        prompt = f"La domanda era: '{current_question}'. L'utente ha risposto: '{user_answer}'. Qual è il punteggio (1-7)?"
        try:
            response = self.chat.send_message(prompt)
            data = json.loads(response.text)
            return data.get("score")
        except Exception as e:
            print(f"Errore di parsing: {e}")
            return None
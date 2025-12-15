import json
from google import genai


class Assistant:
    def __init__(self, personality_config):
        self.client = genai.Client()
        
        self.gestures = [
            "Smile", "BigSmile", "Wink", "Thoughtful", "Surprise", 
            "Oh", "BrowFrown", "BrowRaise", "Blink", "Nod", 
            "Shake", "Roll", "GazeAway", "ExpressDisgust", 
            "ExpressSad", "ExpressAnger", "ExpressFear"
        ]

        technical_instruction = f"""
        TECXHNICAL INSTRUCTIONS:
        You are controlling a physical robot head called Furhat.
        
        AVAILABLE GESTURES:
        {', '.join(self.gestures)}

        INSTRUCTIONS:
        1. Always answer in JSON format.
        2. Decide which gesture matches your text and the mood of the conversation you are given in the context. Use "Nod" for agreement, "Shake" for disagreement, "Smile" for greetings, etc.
        3. If no gesture is needed, use null or "None".
        4. "text": The sentence you want to speak.
        5. "gesture": The name of the gesture to perform, or null if no gesture is needed.
        6. "end_conversation": Boolean, true only if the user wants to stop.

        Example JSON output:
        {{
            "text": "Hello! It is so nice to meet you.",
            "gesture": "BigSmile",
            "end_conversation": false
        }}
        """

        fullConfig = f"{personality_config}\n\n{technical_instruction}"

        self.chat = self.client.chats.create(model='gemini-2.5-flash', config=fullConfig)
        
    def getResponse(self, user_input):
        try:
            response = self.chat.send_message(user_input)
            response_data = json.loads(response.text)
            return response_data

        except Exception as e:
            print(f"Errore AI: {e}")
            # Fallback in caso di errore
            return {"text": "I am having trouble thinking right now.", "gesture": "BrowFrown", "end_conversation": False}
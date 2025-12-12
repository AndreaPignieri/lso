from google import genai


class Assistant:
    def __init__(self, config):
        self.client = genai.Client()
        self.chat = self.client.chats.create(model='gemini-2.5-flash', config=config)
    def getResponse(self, user_input):
        response = self.chat.send_message(role='user', content=user_input)
        return response.text
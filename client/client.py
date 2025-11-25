import socket
import json
import sys
import logging
from furhat_realtime_api import FurhatClient


# Configurazione (deve corrispondere al main.c del server)
SERVER_IP = "127.0.0.1"  # Usa "localhost" se sei sulla stessa macchina
SERVER_PORT = 8080

furhat = FurhatClient("127.0.0.1")  # Add authentication key as second argument if needed
furhat.set_logging_level(logging.INFO)  # Use logging.DEBUG for more details
furhat.connect()
furhat.request_speak_text("Hello world, I am Furhat.")
furhat.disconnect()
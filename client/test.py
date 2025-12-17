import unittest
import socket
import json
import time

# Configurazione
SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080

class TestPersonalityLogic(unittest.TestCase):
    
    def _send_request(self, request):
        """Helper per mandare richieste al server C"""
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.settimeout(2)
                s.connect((SERVER_IP, SERVER_PORT))
                s.sendall(json.dumps(request).encode('utf-8'))
                data = s.recv(4096)
                if not data: return None
                return json.loads(data.decode('utf-8'))
        except Exception as e:
            self.fail(f"Errore connessione server: {e}")

    def test_01_get_questions(self):
        print("\n--- TEST 1: Get Questions ---")
        response = self._send_request({"type": "get_questions"})
        
        # Se fallisce qui, stampiamo l'intero oggetto response per capire perché
        self.assertEqual(response.get("status"), "success", f"Errore Server: {response}")
        self.assertTrue(len(response["questions"]) == 10)
        self.assertIn("estroversa", response["questions"][0])
        print("✅ Domande ricevute OK.")

    def test_02_nervous(self):
        """Target: NERVOUS"""
        print("--- TEST 2: Personality NERVOUS ---")
        scores = [4, 4, 4, 7, 4, 4, 4, 4, 1, 4] 
        
        res = self._send_request({"type": "tipi_submission", "scores": scores})
        
        # --- MODIFICA CHIAVE QUI ---
        status = res.get("status", "unknown")
        server_msg = res.get("message", "Nessun messaggio dal server")
        
        # Questo messaggio apparirà SOLO se il test fallisce
        self.assertEqual(status, "success", f"FALLITO! Motivo server: '{server_msg}'")

        config = res.get("config", "")
        self.assertIn("Nervous", config)
        print("✅ Triggered: NERVOUS")

    def test_03_timid(self):
        """Target: TIMID"""
        print("--- TEST 3: Personality TIMID ---")
        scores = [1, 4, 4, 1, 4, 7, 4, 4, 7, 4]
        
        res = self._send_request({"type": "tipi_submission", "scores": scores})
        
        status = res.get("status", "unknown")
        server_msg = res.get("message", "Nessun messaggio dal server")
        
        self.assertEqual(status, "success", f"FALLITO! Motivo server: '{server_msg}'")

        config = res.get("config", "")
        self.assertIn("Timid", config)
        print("✅ Triggered: TIMID")

    def test_04_open(self):
        """Target: OPEN"""
        print("--- TEST 4: Personality OPEN ---")
        scores = [4, 4, 4, 1, 7, 4, 4, 4, 7, 1]

        res = self._send_request({"type": "tipi_submission", "scores": scores})

        status = res.get("status", "unknown")
        server_msg = res.get("message", "Nessun messaggio dal server")
        
        self.assertEqual(status, "success", f"FALLITO! Motivo server: '{server_msg}'")

        config = res.get("config", "")
        self.assertIn("Open", config)
        print("✅ Triggered: OPEN")

    def test_05_relaxed(self):
        """Target: RELAXED"""
        print("--- TEST 5: Personality RELAXED ---")
        scores = [4, 1, 4, 1, 4, 4, 7, 4, 7, 4]
        
        res = self._send_request({"type": "tipi_submission", "scores": scores})
        
        status = res.get("status", "unknown")
        server_msg = res.get("message", "Nessun messaggio dal server")
        
        self.assertEqual(status, "success", f"FALLITO! Motivo server: '{server_msg}'")

        config = res.get("config", "")
        self.assertIn("Relaxed", config)
        print("✅ Triggered: RELAXED")

    def test_06_serious(self):
        """Target: SERIOUS"""
        print("--- TEST 6: Personality SERIOUS ---")
        scores = [4, 4, 7, 1, 4, 4, 4, 1, 7, 4]
        
        res = self._send_request({"type": "tipi_submission", "scores": scores})
        
        status = res.get("status", "unknown")
        server_msg = res.get("message", "Nessun messaggio dal server")
        
        self.assertEqual(status, "success", f"FALLITO! Motivo server: '{server_msg}'")

        config = res.get("config", "")
        self.assertIn("Serious", config)
        print("✅ Triggered: SERIOUS")

    def test_07_neutral(self):
        """Target: NEUTRAL"""
        print("--- TEST 7: Personality NEUTRAL ---")
        scores = [4, 4, 4, 4, 4, 4, 4, 4, 4, 4]
        
        res = self._send_request({"type": "tipi_submission", "scores": scores})
        
        status = res.get("status", "unknown")
        server_msg = res.get("message", "Nessun messaggio dal server")
        
        self.assertEqual(status, "success", f"FALLITO! Motivo server: '{server_msg}'")

        config = res.get("config", "")
        self.assertIn("Neutral", config)
        print("✅ Triggered: NEUTRAL")
        
    def test_08_invalid_input(self):
        """Testa gestione errori"""
        print("--- TEST 8: Error Handling ---")
        res = self._send_request({"type": "tipi_submission", "scores": []})
        
        # Qui ci aspettiamo "error", quindi usiamo assertEqual("error")
        status = res.get("status", "unknown")
        self.assertEqual(status, "error", f"Doveva fallire ma è successo: {status}")
        print("✅ Error handled (Empty Array)")

if __name__ == '__main__':
    unittest.main(verbosity=0)
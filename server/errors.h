#ifndef ERRORS_H
#define ERRORS_H

#define JSON_ERROR "Errore formato JSON"
#define DATA_RECEPTION_ERROR "Errore ricezione dati"
#define REQUEST_PROCESSING_ERROR "Errore processamento richiesta"
#define RESPONSE_CREATION_ERROR "Errore creazione risposta"

// La funzione accetta un messaggio stringa, non un numero
void errorHandler(int clientSocket, const char* errorMessage);

#endif
#ifndef ERRORS_H
#define ERRORS_H

#define JSON_ERROR 10
#define DATA_RECEPTION_ERROR 20
#define INVALID_RESPONSES 30

void errorHandler(int clientSocket, const char* errorMessage);

#endif // ERRORS_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#include "functions.h"

#define TRUE 1

int main()
{
    int sockFD = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    if (sockFD < 0) 
    {
        perror("Socket creation failed"), exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8080);
    
    if (bind(sockFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        perror("Binding failed"), exit(EXIT_FAILURE);
    }

    if (listen(sockFD, 5) < 0) 
    {
        perror("Listening failed"), exit(EXIT_FAILURE);
    }

    while (TRUE) {
        struct sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(sockFD, (struct sockaddr*)&clientAddress, &clientLength);
        
        if (clientSocket < 0) {
            perror("Accepting connection failed"), exit(EXIT_FAILURE);
        }

        int *clientSockPtr = malloc(sizeof(int));
        if (!clientSockPtr) {
            perror("malloc failed"), exit(EXIT_FAILURE);
        }
        *clientSockPtr = clientSocket;

        pthread_t clientThread;

        if (pthread_create(&clientThread, NULL, clientHandler, (void*)clientSockPtr) != 0) {
            perror("Thread creation failed"), free(clientSockPtr), exit(EXIT_FAILURE);
        }

        pthread_detach(clientThread);
    }

    close(sockFD);
    return 0;
}
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

    while(TRUE)
    {
        int clientSocket;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        clientSocket = accept(sockFD, (struct sockaddr*)&client_addr, &client_len);
        if (clientSocket < 0) 
        {
            perror("Accepting connection failed"), exit(EXIT_FAILURE);
        }

        pthread_t client_thread;
        
        if (pthread_create(&client_thread, NULL, clientHandler, (void*)&clientSocket) != 0) 
        {
            perror("Thread creation failed"), exit(EXIT_FAILURE);
        }
    }
    
    return 0;
}
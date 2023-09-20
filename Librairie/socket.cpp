#include "socket.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int ServerSocket(int port)
{
    int s;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Erreur de socket()");
        exit(1);
    }

    struct addrinfo *results;
    struct addrinfo hints;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // pour une connexion passive

    char port_str[10];
    sprintf(port_str, "%d", port);


    if (getaddrinfo(NULL,port_str,&hints,&results) != 0)
    {
        close(s);
        exit(1);
    }

    if (bind(s,results->ai_addr,results->ai_addrlen) < 0)
    {
        perror("Erreur de bind()");
        exit(1);
    }

    return s;
}
int Accept(int sEcoute,char *ipClient)
{
    if(listen(sEcoute, SOMAXCONN) != 0)
    {
        perror("Erreur listen");
        exit(1);
    }

    struct sockaddr *addr;
    int sService;

    if((sService = accept(sEcoute, NULL, NULL)) == -1)
    {
        perror("Erreur accpet");
        exit(1);
    }

    char port[NI_MAXSERV];
    char host[NI_MAXHOST];

    struct sockaddr_in adrClient;
    socklen_t adrClientLen;
    getpeername(sService,(struct sockaddr*)&adrClient,&adrClientLen);
    getnameinfo((struct 
    sockaddr*)&adrClient,adrClientLen,ipClient,NI_MAXHOST,port,NI_MAXSERV,NI_NUMERICSERV | 
    NI_NUMERICHOST);
    printf("Client connecte --> Adresse IP: %s\n",ipClient);

    return sService;



}
int ClientSocket(char* ipServeur,int portServeur)
{
    int sClient;

    if ((sClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Erreur de socket()");
        exit(1);
    }


    struct addrinfo hints;
    struct addrinfo *results;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;


    char port[10];

    sprintf(port, "%d", portServeur);

    if (getaddrinfo(ipServeur,port,&hints,&results) != 0)
        exit(1);

    if (connect(sClient,results->ai_addr,results->ai_addrlen) == -1)
    {
        perror("Erreur de connect()");
        exit(1);
    }
    printf("connect() reussi !\n");




    return sClient;
}
int Send(int sSocket,char* data,int taille)
{
    return 0;
}
int Receive(int sSocket,char* data)
{
    return 0;
}

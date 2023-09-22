#include "../Librairie/socket.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{

    int sClient;
    char ipServeur[20];

    strcpy(ipServeur, "192.168.203.128");


    sClient = ClientSocket(ipServeur, 1056);


    char data[100];

    strcpy(data, "potty le bot");


    Send(sClient, data, strlen(data));


    return 0;
}
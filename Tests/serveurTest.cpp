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
    //SERVEURSOCKET//

    int s;

    s = ServerSocket(1084);

    printf("Socket : %d\n", s);

    int sService;
    char ipClient[NI_MAXHOST]; // Allouez suffisamment de mémoire pour stocker l'adresse IP

    sService = Accept(s, ipClient);


    printf("Service socket : %d\n", sService);

    printf("ipClient : %s\n", ipClient);


    return 0;
}
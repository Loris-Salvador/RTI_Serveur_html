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
    char *ipClient;

    sService = Accept(s, ipClient);


    printf("Service socket : %d\n", sService);

    printf("ipClient : %s\n", ipClient);

    char data[200];

    Receive(sService,data);

    printf("\n\ndata lu : %s", data);


    return 0;
}
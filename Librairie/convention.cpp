#include "convention.h"
#include <string.h>


void LoginRequestToString(const char* user, const char* password, bool isNewClient, char* requete)
{
    strcpy(requete, LOGIN);
    strcat(requete, CS);
    strcat(requete, user);
    strcat(requete, CS);
    strcat(requete, password);
    strcat(requete, CS);
    if(isNewClient)
        strcat(requete, "1");
    else
        strcat(requete, "0");
}

#include "convention.h"
#include <string.h>
#include <cstdio>


void LoginRequestToString(char* myString, const char* user, const char* password, bool isNewClient)
{
    sprintf(myString, "%s%s%s%s%s%s%d", LOGIN, CS, user, CS, password, CS, isNewClient);
}


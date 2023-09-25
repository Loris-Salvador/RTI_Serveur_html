#ifndef CONVENTION_H
#define CONVENTION_H

#define CS "#"
#define LOGIN "LOGIN"
#define IP_SERVEUR "192.168.203.128"
#define PORT_SERVEUR 1025

void LoginRequestToString(const char* user, const char* password, bool isNewClient, char* requete);



#endif
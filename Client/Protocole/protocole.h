#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#define CS "#"
#define LOGIN "LOGIN"
#define CONSULT "CONSULT"
#define ACHAT "ACHAT"
#define CADDIE "CADDIE"
#define IP_SERVEUR "192.168.203.128"
#define PORT_SERVEUR 1024

void LoginRequestToString(char* requete, const char* user, const char* password, bool isNewClient);



#endif
#include "OVESP.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>




bool is_Log_In_BD(char* user, MYSQL* connexion);
bool OVESP_Login(char* user, char* password, MYSQL* connexion);
void add_Client_In_BD(char* user, char* password, MYSQL* connexion);

void OVESP_Consult(int idArticle, ARTICLE* art);

void OVESP_Achat(int idArticle, int quantite, ARTICLE* art);

int OVESP_Caddie(ARTICLE** panier);

bool OVESP_Cancel(int idArticle, MYSQL* connexion);

void OVESP_CancelAll();

int OVESP_Confirmer();

void OVESP_Logout();


ARTICLE ** caddie;



//***** Parsing de la requete et creation de la reponse *************
bool OVESP(char* requete, char* reponse,int socket, ARTICLE** cadd, MYSQL* connexion)
{

    MYSQL_ROW ligne;
    char requete_sql[200];
    MYSQL_RES  *resultat;

    caddie = cadd;
    // ***** Récupération nom de la requete *****************
    char *ptr = strtok(requete,"#");

    // ***** LOGIN ******************************************
    if (strcmp(ptr,"LOGIN") == 0) 
    {
        char user[50], password[50];

        strcpy(user,strtok(NULL,"#"));
        strcpy(password,strtok(NULL,"#"));

        int newclient = atoi(strtok(NULL,"#"));

        
        printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);
        
        if(!newclient) // Client existant dans la BD
        {
            if (OVESP_Login(user,password, connexion))
            {
                sprintf(reponse,"LOGIN#OK#Client Log");
            } 
            else
            {
                if(is_Log_In_BD(user, connexion))
                    sprintf(reponse,"LOGIN#BAD#Mauvais mot de passe !");
                else
                    sprintf(reponse,"LOGIN#BAD#Client n'existe pas !");

                return false;
            }
        }
        else if (!is_Log_In_BD(user, connexion)) // New Client qui n'est pas dejà dans la BD
        {
            add_Client_In_BD(user, password, connexion);
            sprintf(reponse,"LOGIN#OK#Nouveau Client");
        }
        else // Ce "user" est déjà utilisé
            return false;
    
    }
    // ***** LOGOUT *****************************************
    else if (strcmp(ptr,"LOGOUT") == 0)
    {
        printf("\t[THREAD %p] LOGOUT\n",pthread_self());
        sprintf(reponse,"LOGOUT#OK");
        return false;
    }

    // ***** CONSULT *****************************************
    else if (strcmp(ptr,"CONSULT") == 0)
    {

        printf("\t[THREAD %p] CONSULT\n",pthread_self());
        // Acces BD
        sprintf(requete_sql,"select * from ARTICLE");

        int idArticle = atoi(strtok(NULL,"#"));
        
        if(mysql_query(connexion,requete_sql) != 0)
        {
            fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
            exit(1);
        }

        if((resultat = mysql_store_result(connexion))==NULL)
        {
            fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
            exit(1);
        }

        while ((ligne = mysql_fetch_row(resultat)) != NULL && atoi(ligne[0]) != idArticle);//recherche du bon article en fct de l'id
                             
        if(ligne != NULL && atoi(ligne[0]) == idArticle)
        {
            sprintf(reponse,"CONSULT#%s#%s#%s#%s#%s",ligne[0],ligne[1],ligne[3],ligne[4],ligne[2]);
        }
        else
        {
            sprintf(reponse,"CONSULT#-1#NotFound");
        }           
    }

    // ***** ACHAT *****************************************
    else if (strcmp(ptr,"ACHAT") == 0)
    {
        printf("\t[THREAD %p] ACHAT\n",pthread_self());

        sprintf(requete_sql,"select * from ARTICLE");
        
        if(mysql_query(connexion,requete_sql) != 0)
        {
            fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
            exit(1);
        }


        if((resultat = mysql_store_result(connexion))==NULL)
        {
            fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
            exit(1);
        }

        int idArticle = atoi(strtok(NULL,"#"));

        while ((ligne = mysql_fetch_row(resultat)) != NULL && atoi(ligne[0]) != idArticle);//recherche du bon article en fct de l'id

        if(ligne != NULL && atoi(ligne[0]) == idArticle)// est-ce qu'on la trouvé ?           
        {
            int stock=atoi(ligne[3]);
            int quantite = atoi(strtok(NULL,"#"));

            if(quantite<=stock)
            {
                sprintf(requete_sql,"update ARTICLE set stock=%d where id=%d", stock-quantite, atoi(ligne[0]));
                 
                if(mysql_query(connexion,requete_sql) != 0)
                {
                    fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
                    exit(1);
                }

                sprintf(reponse,"ACHAT#%s#%d#%s#%s",ligne[0],quantite,ligne[1],ligne[2]);

                ARTICLE art;

                art.id = atoi(ligne[0]);
                strcpy(art.intitule, ligne[1]);
                art.stock = quantite;
                strcpy(art.image, ligne[4]);
                art.prix = atof(ligne[2]);

                printf("art: %f\n", art.prix);

                int i;
                for (i = 0; caddie[i] != NULL && i<10; i++);

                if(caddie[i] == NULL && i<10)//Vérification inutile mais on sait jamais
                {
                    caddie[i] = (ARTICLE *)malloc(sizeof(ARTICLE));
                    *caddie[i] = art;
                    printf("Dans la vérif\n");
                }

            }
            else
                sprintf(reponse,"ACHAT#%s#0",ligne[0]);
        }
        else
            sprintf(reponse,"ACHAT#-1");

    }

    // ***** CADDIE *****************************************
    else if (strcmp(ptr,"CADDIE") == 0)
    {
        printf("\t[THREAD %p] CADDIE\n",pthread_self());

        sprintf(reponse,"CADDIE#OK"); // Chaine pour stocker les éléments concaténés
        
        int i;

        puts("Dans CADDIE");
        for (i = 0; caddie[i] != NULL; i++) {
            printf("I: %d  prix: %f\n",i,caddie[i]->prix);

            sprintf(reponse + strlen(reponse), "#%d#%s#%.2f#%d#%s", caddie[i]->id, caddie[i]->intitule, caddie[i]->prix, caddie[i]->stock, caddie[i]->image);
        }
    }

    // ***** CANCEL *****************************************
    else if (strcmp(ptr,"CANCEL") == 0)
    {
        printf("\t[THREAD %p] CANCEL\n",pthread_self());

        int idArticle = atoi(strtok(NULL,"#"));

        if(OVESP_Cancel(idArticle, connexion))
            sprintf(reponse,"CANCEL#OK");
        else
            sprintf(reponse,"CANCEL#BAD");
    }

    // ***** CANCEL ALL *****************************************

    else if (strcmp(ptr,"CANCEL ALL") == 0)
    {
        printf("\t[THREAD %p] CANCEL ALL\n",pthread_self());

        int i;

        for (i = 0; caddie[i] != NULL; i++)
            OVESP_Cancel(i, connexion);

        sprintf(reponse,"CANCEL ALL#OK");
    }

    return true;
}


bool OVESP_Cancel(int idArticle, MYSQL* connexion)
{

    MYSQL_ROW ligne;
    char requete_sql[200];
    MYSQL_RES  *resultat;


    sprintf(requete_sql,"select * from ARTICLE");
        
    if(mysql_query(connexion,requete_sql) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        exit(1);
    }

    if((resultat = mysql_store_result(connexion))==NULL)
    {
        fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
        exit(1);
    }


    while ((ligne = mysql_fetch_row(resultat)) != NULL && atoi(ligne[0]) != idArticle);//recherche du bon article en fct de l'id


    if(ligne != NULL && atoi(ligne[0]) == idArticle)// est-ce qu'on la trouvé ?           
    {
        int stock=atoi(ligne[3]);
        int i;

        for (i = 0; caddie[i]->id != idArticle; i++);

        sprintf(requete_sql,"update ARTICLE set stock=%d where id=%d", stock+caddie[i]->stock, atoi(ligne[0]));
                 
        if(mysql_query(connexion,requete_sql) != 0)
        {
            fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
            exit(1);
        }

        free(caddie[i]);

        return true;
    }
    else
        return false;
}



bool is_Log_In_BD(char* user, MYSQL* connexion)
{
    MYSQL_ROW ligne;
    char requete_sql[200];
    MYSQL_RES  *resultat;

    sprintf(requete_sql, "SELECT * FROM USER WHERE USERNAME LIKE '%s'", user);

        
    if(mysql_query(connexion,requete_sql) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        return false;
    }


    if((resultat = mysql_store_result(connexion))==NULL)
    {
        fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
        return false;
    }


    ligne = mysql_fetch_row(resultat);

    if(ligne == NULL)
    {
        return false;
    }

    char user_bd[200];

    strcpy(user_bd, ligne[0]);

    if(strcmp(user_bd, user)==0)
    {
        return true;
    }


    return false;
}
bool OVESP_Login(char* user, char* password, MYSQL* connexion)
{
    MYSQL_ROW ligne;
    char requete_sql[200];
    MYSQL_RES  *resultat;
    char password_bd[200];


    sprintf(requete_sql, "SELECT * FROM USER WHERE USERNAME LIKE '%s'", user);

        
    if(mysql_query(connexion,requete_sql) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
        return false;
    }


    if((resultat = mysql_store_result(connexion))==NULL)
    {
        fprintf(stderr, "Erreur de mysql_store_result: %s\n",mysql_error(connexion));
        return false;
    }


    ligne = mysql_fetch_row(resultat);

    if(ligne == NULL)
    {
        return false;
    }

    printf("\n ligne 1 : %s\n", ligne[1]);
    puts(password);

    strcpy(password_bd, ligne[1]);

    if(strcmp(password_bd, password)==0)
    {
        return true;
    }


    return false;
}

void add_Client_In_BD(char* user, char* password, MYSQL* connexion)
{
    char requete_sql[200];

    sprintf(requete_sql, "INSERT INTO USER VALUES ('%s', '%s')", user, password);

        
    if(mysql_query(connexion,requete_sql) != 0)
    {
        fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
    }
}
#include "OVESP.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mysql.h>




//bool is_Log_In_BD(char* user);
//bool OVESP_Login(char* user, char* password);
//void add_Client_In_BD(char* user, char* password);

void OVESP_Consult(int idArticle, ARTICLE* art);

void OVESP_Achat(int idArticle, int quantite, ARTICLE* art);

int OVESP_Caddie(ARTICLE** panier);

bool OVESP_Cancel(int idArticle, MYSQL* connexion);

void OVESP_CancelAll();

int OVESP_Confirmer();

void OVESP_Logout();

MYSQL_ROW ligne;
char requete_sql[200];
MYSQL_RES  *resultat;
ARTICLE ** caddie;



//***** Parsing de la requete et creation de la reponse *************
bool OVESP(char* requete, char* reponse,int socket, ARTICLE** cadd, MYSQL* connexion)
{

    caddie = cadd;
    // ***** Récupération nom de la requete *****************
    char *ptr = strtok(requete,"#");

    // ***** LOGIN ******************************************
    // if (strcmp(ptr,"LOGIN") == 0) 
    // {
    //     char user[50], password[50], myBool;
        
    //     strcpy(user,strtok(NULL,"#"));
    //     strcpy(password,strtok(NULL,"#"));

    //     int newclient = atoi(strcpy(password,strtok(NULL,"#")));
        
    //     printf("\t[THREAD %p] LOGIN de %s\n",pthread_self(),user);
        
    //     if(!newclient) // Client existant dans la BD
    //     {
    //         if (OVESP_Login(user,password))
    //         {
    //             sprintf(reponse,"LOGIN#OK#Client Log");
    //         } 
    //         else
    //         {
    //             if(is_Log_In_BD(user))
    //                 sprintf(reponse,"LOGIN#BAD#Mauvais mot de passe !");
    //             else
    //                 sprintf(reponse,"LOGIN#BAD#Client n'existe pas !");

    //             return false;
    //         }
    //     }
    //     else if (!is_Log_In_BD(user)) // New Client qui n'est pas dejà dans la BD
    //     {
    //         add_Client_In_BD(user, password);
    //         sprintf(reponse,"LOGIN#OK#Nouveau Client");
    //     }
    //     else // Ce "user" est déjà utilisé
    //         return false;
    // }
    
    // ***** LOGOUT *****************************************
    if (strcmp(ptr,"LOGOUT") == 0)
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
        sprintf(requete_sql,"select * from UNIX_FINAL");

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

        sprintf(requete_sql,"select * from UNIX_FINAL");
        
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
                sprintf(requete_sql,"update UNIX_FINAL set stock=%d where id=%d", stock-quantite, atoi(ligne[0]));
                 
                if(mysql_query(connexion,requete_sql) != 0)
                {
                    fprintf(stderr, "Erreur de mysql_query: %s\n",mysql_error(connexion));
                    exit(1);
                }

                sprintf(reponse,"ACHAT#%s#%d#%s",ligne[0],quantite,ligne[2]);

                int i;
                for (i = 0; caddie[i] != NULL && i<10; i++);

                if(caddie[i] != NULL && i<10)//Vérification inutile mais on sait jamais
                {
                    caddie[i] = (ARTICLE *)malloc(sizeof(ARTICLE));
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

        for (i = 0; caddie[i]->id != NULL; i++) {
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

        for (i = 0; caddie[i]->id != NULL; i++)
            OVESP_Cancel(i, connexion);

        sprintf(reponse,"CANCEL ALL#OK");
    }

    return true;
}


bool OVESP_Cancel(int idArticle, MYSQL* connexion)
{
    sprintf(requete_sql,"select * from UNIX_FINAL");
        
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

        sprintf(requete_sql,"update UNIX_FINAL set stock=%d where id=%d", stock+caddie[i]->stock, atoi(ligne[0]));
                 
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
#include "windowclient.h"
#include "ui_windowclient.h"
#include <QMessageBox>
#include <string>
#include "../LibSocket/socket.h"
#include "./Protocole/protocole.h"
#include <signal.h>
#include <csignal>
#include <unistd.h>


using namespace std;

extern WindowClient *w;

int CurrentIdArticle = 0;
int sClient;
bool alarmeActivee = false;
bool logged = false;

#define REPERTOIRE_IMAGES "images/"

void HandlerSIGALRM(int sig);


WindowClient::WindowClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::WindowClient)
{
    ui->setupUi(this);

    // Configuration de la table du panier (ne pas modifer)
    ui->tableWidgetPanier->setColumnCount(3);
    ui->tableWidgetPanier->setRowCount(0);
    QStringList labelsTablePanier;
    labelsTablePanier << "Article" << "Prix à l'unité" << "Quantité";
    ui->tableWidgetPanier->setHorizontalHeaderLabels(labelsTablePanier);
    ui->tableWidgetPanier->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetPanier->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetPanier->horizontalHeader()->setVisible(true);
    ui->tableWidgetPanier->horizontalHeader()->setDefaultSectionSize(160);
    ui->tableWidgetPanier->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetPanier->verticalHeader()->setVisible(false);
    ui->tableWidgetPanier->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    ui->pushButtonPayer->setText("Confirmer achat");
    setPublicite("!!! Bienvenue sur le Maraicher en ligne !!!");

    // Exemples à supprimer



    // Armement des signaux

    struct sigaction A;
    A.sa_handler =HandlerSIGALRM;
    sigemptyset(&A.sa_mask);
    A.sa_flags = 0;

    if(sigaction(SIGALRM,&A,NULL) == -1)
    {
      perror("Erreur de sigaction");
      exit(1);
    }


}

WindowClient::~WindowClient()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setNom(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditNom->clear();
    return;
  }
  ui->lineEditNom->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getNom()
{
  strcpy(nom,ui->lineEditNom->text().toStdString().c_str());
  return nom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setMotDePasse(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditMotDePasse->clear();
    return;
  }
  ui->lineEditMotDePasse->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getMotDePasse()
{
  strcpy(motDePasse,ui->lineEditMotDePasse->text().toStdString().c_str());
  return motDePasse;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setPublicite(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditPublicite->clear();
    return;
  }
  ui->lineEditPublicite->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setImage(const char* image)
{
  // Met à jour l'image
  char cheminComplet[80];
  sprintf(cheminComplet,"%s%s",REPERTOIRE_IMAGES,image);
  QLabel* label = new QLabel();
  label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label->setScaledContents(true);
  QPixmap *pixmap_img = new QPixmap(cheminComplet);
  label->setPixmap(*pixmap_img);
  label->resize(label->pixmap()->size());
  ui->scrollArea->setWidget(label);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::isNouveauClientChecked()
{
  if (ui->checkBoxNouveauClient->isChecked()) return 1;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setArticle(const char* intitule,float prix,int stock,const char* image)
{
  ui->lineEditArticle->setText(intitule);
  if (prix >= 0.0)
  {
    char Prix[20];
    sprintf(Prix,"%.2f",prix);
    ui->lineEditPrixUnitaire->setText(Prix);
  }
  else ui->lineEditPrixUnitaire->clear();
  if (stock >= 0)
  {
    char Stock[20];
    sprintf(Stock,"%d",stock);
    ui->lineEditStock->setText(Stock);
  }
  else ui->lineEditStock->clear();
  setImage(image);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getQuantite()
{
  return ui->spinBoxQuantite->value();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setTotal(float total)
{
  if (total >= 0.0)
  {
    char Total[20];
    sprintf(Total,"%.2f",total);
    ui->lineEditTotal->setText(Total);
  }
  else ui->lineEditTotal->clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::loginOK()
{
  ui->pushButtonLogin->setEnabled(false);
  ui->pushButtonLogout->setEnabled(true);
  ui->lineEditNom->setReadOnly(true);
  ui->lineEditMotDePasse->setReadOnly(true);
  ui->checkBoxNouveauClient->setEnabled(false);

  ui->spinBoxQuantite->setEnabled(true);
  ui->pushButtonPrecedent->setEnabled(true);
  ui->pushButtonSuivant->setEnabled(true);
  ui->pushButtonAcheter->setEnabled(true);
  ui->pushButtonSupprimer->setEnabled(true);
  ui->pushButtonViderPanier->setEnabled(true);
  ui->pushButtonPayer->setEnabled(true);

  logged = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::logoutOK()
{
  ui->pushButtonLogin->setEnabled(true);
  ui->pushButtonLogout->setEnabled(false);
  ui->lineEditNom->setReadOnly(false);
  ui->lineEditMotDePasse->setReadOnly(false);
  ui->checkBoxNouveauClient->setEnabled(true);

  ui->spinBoxQuantite->setEnabled(false);
  ui->pushButtonPrecedent->setEnabled(false);
  ui->pushButtonSuivant->setEnabled(false);
  ui->pushButtonAcheter->setEnabled(false);
  ui->pushButtonSupprimer->setEnabled(false);
  ui->pushButtonViderPanier->setEnabled(false);
  ui->pushButtonPayer->setEnabled(false);

  setNom("");
  setMotDePasse("");
  ui->checkBoxNouveauClient->setCheckState(Qt::CheckState::Unchecked);

  setArticle("",-1.0,-1,"");

  w->videTablePanier();
  w->setTotal(-1.0);

  logged = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table du panier (ne pas modifier) /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::ajouteArticleTablePanier(const char* article,float prix,int quantite)
{
    char Prix[20],Quantite[20];

    sprintf(Prix,"%.2f",prix);
    sprintf(Quantite,"%d",quantite);

    // Ajout possible
    int nbLignes = ui->tableWidgetPanier->rowCount();
    nbLignes++;
    ui->tableWidgetPanier->setRowCount(nbLignes);
    ui->tableWidgetPanier->setRowHeight(nbLignes-1,10);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(article);
    ui->tableWidgetPanier->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Prix);
    ui->tableWidgetPanier->setItem(nbLignes-1,1,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Quantite);
    ui->tableWidgetPanier->setItem(nbLignes-1,2,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::videTablePanier()
{
    ui->tableWidgetPanier->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getIndiceArticleSelectionne()
{
    QModelIndexList liste = ui->tableWidgetPanier->selectionModel()->selectedRows();
    if (liste.size() == 0) return -1;
    QModelIndex index = liste.at(0);
    int indice = index.row();
    return indice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueMessage(const char* titre,const char* message)
{
   QMessageBox::information(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueErreur(const char* titre,const char* message)
{
   QMessageBox::critical(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// CLIC SUR LA CROIX DE LA FENETRE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::closeEvent(QCloseEvent *event)
{
  if(!logged)
    on_pushButtonLogout_clicked();
  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogin_clicked()
{
  alarmeActivee = false;
  const char *user = getNom();
  const char *password = getMotDePasse();
  bool isNewClient = isNouveauClientChecked();

  bool boucle = true;

  char requete[200], reponse[200];

  LoginRequestToString(requete, user, password, isNewClient);

  char ipServeur[20];

  strcpy(ipServeur, IP_SERVEUR);

  sClient = ClientSocket(ipServeur, PORT_SERVEUR);
  Send(sClient, requete, strlen(requete));
  alarm(2);
  Receive(sClient, reponse);
  alarm(0);

  if(alarmeActivee)
  {
    dialogueErreur("Erreur", "File d'attente pleine reessayez plus tard");
    return;
  }

  char *ptr = strtok(reponse,"#");

  char etat[12], message[50];

  strcpy(etat,strtok(NULL,"#"));
  strcpy(message,strtok(NULL,"#"));


  while(boucle)
  {
    boucle = false;
    if (strcmp(ptr,"LOGIN") == 0)
    {
      if (strcmp(etat,"OK") == 0)
      {
        loginOK();
        dialogueMessage("LOGIN",message);
      }
      else if (strcmp(etat,"BAD") == 0)
      {
        dialogueErreur("LOGIN",message);
        return;
      }
      else if (strcmp(etat, "FILE") == 0)
      {
        dialogueMessage("LOGIN", "Vous avez ete place dans la file d'attente Veuillez patientez...");
        Receive(sClient, reponse);
        ptr = strtok(reponse,"#");
        strcpy(etat,strtok(NULL,"#"));
        strcpy(message,strtok(NULL,"#"));

        boucle = true;
      }
      else
      {
        dialogueErreur(ptr,message);
      }
    }
    else
    {
      dialogueErreur(ptr,message);
    }
  }
  

  AfficherArticle(CurrentIdArticle);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogout_clicked()
{
  char requete[200];
  char reponse[200];

  sprintf(requete, "%s", CANCEL_ALL);

  Send(sClient, requete, strlen(requete));
  Receive(sClient, reponse);

  strtok(reponse,"#");
  char OK [3];
  strcpy(OK, strtok(NULL,"#"));




  strcpy(requete, "LOGOUT");

  Send(sClient, requete, strlen(requete));
  Receive(sClient, reponse);

  char etat[4];

  char *ptr = strtok(reponse,"#");
  strcpy(etat,strtok(NULL,"#"));

  if(strcmp(etat, "BAD") == 0)
  {
    dialogueErreur(ptr,"ERROR");
    return;
  }

  CloseSocket(sClient);
  

  logoutOK();





  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSuivant_clicked()
{
  CurrentIdArticle = (CurrentIdArticle+1)%21;

  AfficherArticle(CurrentIdArticle);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPrecedent_clicked()
{
  CurrentIdArticle = (CurrentIdArticle+20)%21;

  AfficherArticle(CurrentIdArticle);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonAcheter_clicked()
{
  char requete[200], reponse[200];

  if(getQuantite() == 0)
    return;

  sprintf(requete, "%s%s%d%s%d", ACHAT, CS, CurrentIdArticle+1, CS, getQuantite());

  Send(sClient, requete, strlen(requete));
  Receive(sClient, reponse);

  char *ptr = strtok(reponse,"#");
  int idArticle, quantite;
  float prix;
  char intitule[30];

  idArticle = atoi(strtok(NULL,"#"));

  if (strcmp(ptr,"ACHAT") == 0)
  {
    if(idArticle == 0)
    {
      quantite = atoi(strtok(NULL,"#"));
      strcpy(intitule,strtok(NULL,"#"));
      dialogueErreur(ptr, intitule);
    }

    else if (idArticle != -1)
    {
      quantite = atoi(strtok(NULL,"#"));

      if (quantite != 0)
      {
        strcpy(intitule,strtok(NULL,"#"));

        setlocale(LC_NUMERIC, "C");
        prix = atof(strtok(NULL,"#"));
        setlocale(LC_NUMERIC, "");

        ajouteArticleTablePanier(intitule,prix,quantite);

        CurrentIdArticle--;
        on_pushButtonSuivant_clicked();
      }
      else
        dialogueErreur(ptr, "Not Enough Stock");
    }
    else
      dialogueErreur(ptr, "Not Found");
  }
  else
    dialogueErreur(ptr,"Error");

  Actualiser_Panier();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSupprimer_clicked()
{
  char requete[200], reponse[200];

  if(getIndiceArticleSelectionne() == -1)
    return;

  sprintf(requete, "%s%s%d", CANCEL, CS, getIndiceArticleSelectionne());

  Send(sClient, requete, strlen(requete));

  Receive(sClient, reponse);

  char *ptr = strtok(reponse,"#");


  char etat[4];
  strcpy(etat,strtok(NULL,"#"));


  if(strcmp(etat, "OK")==0)
  {
    Actualiser_Panier();
    AfficherArticle(CurrentIdArticle);
  }
  else
    dialogueErreur(ptr,"Error");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonViderPanier_clicked()
{
  char requete[200],reponse[200];
  sprintf(requete, "%s", CANCEL_ALL);

  Send(sClient, requete, strlen(requete));
  Receive(sClient, reponse);

  strtok(reponse,"#");
  char OK [3];
  strcpy(OK, strtok(NULL,"#"));

  if(strcmp(OK,"OK") != 0)
  {
    dialogueErreur("Vider Panier", "Error");
    return;
  }


  Actualiser_Panier();
  AfficherArticle(CurrentIdArticle);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPayer_clicked()
{

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//        NOS FONCTIONS                                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WindowClient::Actualiser_Panier()
{
    videTablePanier();

    char requeteConsult[200], reponseConsult[500];

    sprintf(requeteConsult, "%s", CADDIE);

    Send(sClient, requeteConsult, strlen(requeteConsult));
    Receive(sClient, reponseConsult);

    strtok(reponseConsult,"#");
    
    char OK [3];
    strcpy(OK, strtok(NULL,"#"));

    int count = atoi(strtok(NULL,"#"));

    char id[3], intitule[20], stock[3], image[30];

    float prix, total =0;

    int i;
    for(i = 0; i < count; i++)
    {
      strcpy(id, strtok(NULL,"#"));
      strcpy(intitule, strtok(NULL,"#"));
      strcpy(stock, strtok(NULL,"#"));
      strcpy(image, strtok(NULL,"#"));

      setlocale(LC_NUMERIC, "C");
      prix = atof(strtok(NULL,"#"));
      setlocale(LC_NUMERIC, "");

      total = total + prix*atoi(stock);

      ajouteArticleTablePanier(intitule, prix, atoi(stock));
    }
    
    setTotal(total);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::AfficherArticle(int id)
{
  char requete[200], reponse[200];

  id++;

  sprintf(requete, "%s%s%d", CONSULT, CS, id);

  Send(sClient, requete, strlen(requete));
  Receive(sClient, reponse);

  char *ptr = strtok(reponse,"#");
  char message[50];
  int idArticle;

  idArticle = atoi(strtok(NULL,"#"));
  strcpy(message,strtok(NULL,"#"));

  if (strcmp(ptr,"CONSULT") == 0)
  {
    if (idArticle != -1)
    {
      int stock;
      char image[50];
      float prix;

      stock = atoi(strtok(NULL,"#"));
      strcpy(image,strtok(NULL,"#"));

      setlocale(LC_NUMERIC, "C");
      prix = atof(strtok(NULL,"#"));
      setlocale(LC_NUMERIC, "");


      setArticle(message, prix, stock, image);
    }
    else
      dialogueErreur(ptr,message);
  }
  else
    dialogueErreur(ptr,message);
}


//////////HANDLER///////////////


void HandlerSIGALRM(int sig)
{
  printf("Alarme\n");
  close(sClient);
  alarmeActivee = true;

}


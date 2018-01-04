#include "fenetremenu.h"
#include "fenetreniveau.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include "fenetreregles.h"
#include <QObject>
#include <QMediaPlayer>
#include <QSound>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include "fenetrexsb.h"

FenetreMenu::FenetreMenu()
{
    //Taille de la fenêtre
    setMinimumSize(600, 400);
    //Titre de la fenêtre
    setWindowTitle("THE WAREHOUSE");
    //Fond du menu
    this->setStyleSheet("background-color: black;");

    /* QMediaPlayer* player = new QMediaPlayer(this);
    // player->setMedia(QUrl::fromLocalFile("C:/Users/Nawel Lalioui/Documents/felices.mp3"));
     player->setMedia(QUrl("C:/Users/Nawel Lalioui/Documents/Warehouse/felices.mp3"));
     player->setVolume(50);
     player->play();*/


    //Pour l'image du titre
    QLabel *label = new QLabel(this);
    label->setAlignment(Qt::AlignHCenter);
    label->setPixmap(QPixmap(":/titre.png"));


    //Création du bouton jouer
    m_boutonJouer = new QPushButton(QIcon(":/tileset/perso/tete_sombre.png"), "PLAY");
    //Personnalisation de la police
    QFont Police1("Calibri", 12, QFont::Bold);
    m_boutonJouer->setStyleSheet("background-color: green;");


    m_boutonJouer->setFont(Police1);

    //Création d'un layout
    QVBoxLayout *layout = new QVBoxLayout;

    //Ajout de l'image au layout
    layout->addWidget(label);

    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_boutonJouer);


    //Création du bouton règles
    m_boutonRegles = new QPushButton(QIcon(":/tileset/caisse/verte.png"), "RULES");
    QFont Police2("Calibri", 12, QFont::Bold);
    m_boutonRegles->setFont(Police2);
    m_boutonRegles->setStyleSheet("background-color: yellow;");
    layout->addWidget(m_boutonRegles);

    //Création du bouton charger
    m_boutonCharger = new QPushButton(QIcon(":/file.PNG"), "LOAD FILE");
    QFont Police4("Calibri", 12, QFont::Bold);
    m_boutonCharger->setFont(Police4);
    m_boutonCharger->setStyleSheet("background-color: silver;");
    layout->addWidget(m_boutonCharger);

    //Création du bouton quitter
    m_boutonQuitter = new QPushButton(QIcon(":/tileset/perso/haut_01.png"), "QUIT");
    QFont Police3("Calibri", 12, QFont::Bold);
    m_boutonQuitter->setFont(Police3);
    m_boutonQuitter->setStyleSheet("background-color: silver;");
    layout->addWidget(m_boutonQuitter);


    //Configuration bouton quitter
    QObject::connect(m_boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));
    QObject::connect(m_boutonJouer, SIGNAL(clicked()), this, SLOT(fenNiveau_open()));

    //Configuration bouton règles
    QObject::connect(m_boutonRegles, SIGNAL(clicked()), this, SLOT(fenRegles_open()));

    //Signal et slot pour fermer la fenêtre menu quand on ouvre la fenêtre niveau
    QObject::connect(m_boutonJouer, SIGNAL(clicked()), this, SLOT(close()));
    //Signal et slot pour fermer la fenêtre menu quand on ouvre la fenêtre règles
    QObject::connect(m_boutonRegles, SIGNAL(clicked()), this, SLOT(close()));

    QObject::connect(m_boutonCharger, SIGNAL(clicked()), this, SLOT(charger()));

    //Signal et slot pour fermer la fenêtre menu quand on ouvre la fenêtre load file
    QObject::connect(m_boutonCharger, SIGNAL(clicked()), this, SLOT(close()));


    setLayout(layout);


}

//Fonction pour ouvrir une fenêtre contenant la carte
void FenetreMenu::fenNiveau_open(){
    FenetreNiveau* carte  = new FenetreNiveau(moteur::Carte::charger("../warehouse/carte.txt"));
    carte->showMaximized();
}

//Fonction pour ouvrir une fenêtre contenant les règles
void FenetreMenu::fenRegles_open(){
    FenetreRegles* regles = new FenetreRegles();
    regles->show();
}

void FenetreMenu::charger(){
    QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Fichier XSB (*.xsb)");
    //QMessageBox::information(0, "Fichier", "<font size = 3 color = red > Vous avez sélectionné: \n </font>" + fichier);
    FenetreXSB* fen = new FenetreXSB(fichier);
    fen->show();
}





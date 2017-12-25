#include "fenetremenu.h"
#include "fenetreniveau.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include "fenetreregles.h"

FenetreMenu::FenetreMenu()
{
    //Taille de la fenêtre
    setMinimumSize(600, 400);
    setWindowTitle("MENU");

    //Création du bouton jouer
    m_boutonJouer = new QPushButton(QIcon(":/tileset/perso/tete_sombre.png"), "Play");

    //Création d'un layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_boutonJouer);

    //Création du bouton règles
    m_boutonRegles = new QPushButton(QIcon(":/tileset/caisse/rouge.png"), "Rules");
    layout->addWidget(m_boutonRegles);

    /*//Création du bouton IA
    m_boutonIA = new QPushButton(QIcon(":/tileset/environ/empl_bleu.png"), "AIs");
    layout->addWidget(m_boutonIA);*/

    //Création du bouton quitter
    m_boutonQuitter = new QPushButton(QIcon(":/tileset/perso/haut_01.png"), "Quit");
    layout->addWidget(m_boutonQuitter);

    //Configuration bouton quitter
    QObject::connect(m_boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));
    QObject::connect(m_boutonJouer, SIGNAL(clicked()), this, SLOT(fenNiveau_open()));

    //Configuration bouton règles
    QObject::connect(m_boutonRegles, SIGNAL(clicked()), this, SLOT(fenRegles_open()));


    setLayout(layout);

}

//Fonction pour ouvrir une fenêtre contenant la carte
void FenetreMenu::fenNiveau_open(){

    FenetreNiveau* carte  = new FenetreNiveau(moteur::Carte::charger("../warehouse/carte.txt"));
    carte->show();


}

//Fonction pour ouvrir une fenêtre contenant les règles
void FenetreMenu::fenRegles_open(){
    FenetreRegles* regles = new FenetreRegles();
    regles->show();


}



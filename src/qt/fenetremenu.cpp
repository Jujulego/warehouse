#include "fenetremenu.h"
#include <QPushButton>
#include <QVBoxLayout>
FenetreMenu::FenetreMenu()
{
    //Taille de la fenêtre
    setMinimumSize(600, 400);

    //création du bouton jouer
    m_boutonJouer = new QPushButton(QIcon(":/tileset/caisse/bleue.png"), "Jouer");

    //Création d'un layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_boutonJouer);

    //Création du bouton quitter
    m_boutonQuitter = new QPushButton("Quitter");
    layout->addWidget(m_boutonQuitter);



    setLayout(layout);

}


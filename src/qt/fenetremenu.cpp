#include "fenetremenu.h"
#include <QPushButton>
#include <QHBoxLayout>
FenetreMenu::FenetreMenu()
{
    //Taille de la fenêtre
    setMinimumSize(600, 400);

    //création du boutton jouer
    m_bouttonJouer = new QPushButton(QIcon(":/tileset/caisse/bleue.png"), "Jouer");

    //Création d'un layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_bouttonJouer);
    setLayout(layout);

}


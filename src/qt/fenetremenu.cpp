#include "fenetremenu.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>

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
    m_boutonQuitter = new QPushButton(QIcon(":/tileset/caisse/rouge.png"), "Quitter");
    layout->addWidget(m_boutonQuitter);

    //Configuration bouton quitter
    QObject::connect(m_boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));

    qApp->setStyleSheet("QWidget {background-image: url(./tilset/caisse.bleue.png) }");

    setLayout(layout);

}


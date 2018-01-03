#include "fenetrexsb.h"
#include "fichierxsb.hpp"
#include "fenetreniveau.h"
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>

FenetreXSB::FenetreXSB(QString _fichier)
           :m_fichier(_fichier) {


    //Taille de la fenêtre
    setMinimumSize(600, 400);
    //Titre de la fenêtre
    setWindowTitle("LEVELS");
    //Fond du menu
    this->setStyleSheet("background-color: black;");

    FichierXSB fichierxsb(_fichier.toStdString());

    for (auto niv : fichierxsb.niveaux()) {

        //Création d'un layout
        QHBoxLayout *layout = new QHBoxLayout;

        m_boutonxsb = new QPushButton(QIcon(":/tileset/perso/tete_contour.png"), "LEVEL");
        QFont Police("Calibri", 12, QFont::Bold);
        m_boutonxsb->setFont(Police);
        m_boutonxsb->setStyleSheet("background-color: yellow;");
        layout->addWidget(m_boutonxsb);
        setLayout(layout);

        QObject::connect(m_boutonxsb, &QPushButton::clicked, [=] () {
          niv.carte();
          FenetreNiveau* c  = new FenetreNiveau(niv.carte());
          c->show();

        });



  }

}


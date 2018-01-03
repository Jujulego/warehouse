#include "fenetrexsb.h"
#include "fichierxsb.hpp"
#include "fenetreniveau.h"
#include <QString>
#include <QPushButton>
#include <QvBoxLayout>

FenetreXSB::FenetreXSB(QString _fichier)
           :m_fichier(_fichier) {


    //Taille de la fenêtre
    setMinimumSize(600, 400);
    //Titre de la fenêtre
    setWindowTitle("LEVELS");
    //Fond
    this->setStyleSheet("background-color: black;");

    FichierXSB fichierxsb(_fichier.toStdString());

    QVBoxLayout *layout = new QVBoxLayout;
     layout->setAlignment(Qt::AlignCenter);

     for (auto niv : fichierxsb.niveaux()) {

        m_boutonxsb = new QPushButton(QIcon(":/tileset/perso/tete_contour.png"), "LEVEL");
        QFont Police("Calibri", 8, QFont::Bold);
        m_boutonxsb->setFont(Police);
        m_boutonxsb->setStyleSheet("background-color: yellow;");
        layout->addWidget(m_boutonxsb);
        setLayout(layout);


        QObject::connect(m_boutonxsb, &QPushButton::clicked, [=] () {
          niv.carte();
          FenetreNiveau* c  = new FenetreNiveau(niv.carte());
          c->show();

        });

        //Slot pour fermer fenêtre des niveaux quand on clique sur un niveau
        QObject::connect(m_boutonxsb, SIGNAL(clicked()), this, SLOT(close()));


  }

}


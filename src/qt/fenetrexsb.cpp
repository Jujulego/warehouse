#include "fenetrexsb.h"
#include "fichierxsb.hpp"
#include "fenetreniveau.h"
#include <QString>
#include <QPushButton>

FenetreXSB::FenetreXSB(QString _fichier)
           :m_fichier(_fichier) {
    FichierXSB fichierxsb(_fichier.toStdString());

    for (auto niv : fichierxsb.niveaux()) {
        m_boutonxsb = new QPushButton(QIcon(":/tileset/perso/haut_01.png"), "NIVEAU", this);
        QObject::connect(m_boutonxsb, &QPushButton::clicked, [=] () {
          niv.carte();
          FenetreNiveau* c  = new FenetreNiveau(niv.carte());
          c->show();
        });

  }

}


// Importations
#include "fenetrexsb.h"
#include "fichierxsb.hpp"
#include "fenetreniveau.h"

#include <QString>
#include <QPushButton>
#include <QGridLayout>

// Macros
#define MAX_COL 30

// Constructeur
FenetreXSB::FenetreXSB(QString fichier) {
    // Taille de la fenêtre
    setMinimumSize(600, 400);
    setWindowTitle(QString("LEVELS : ") + fichier);
    setStyleSheet("background-color: black;");

	// Chargement du fichier
    FichierXSB fichierxsb(fichier.toStdString());

    QGridLayout *layout = new QGridLayout;
    QFont police("Calibri", 10, QFont::Bold);

    int i = 0;

    for (auto niv : fichierxsb.niveaux()) {
        QPushButton* boutonxsb = new QPushButton(
        	QIcon(":/tileset/perso/tete_contour.png"),
        	QString("LEVEL ") + QString::number(i + 1)
        );
        boutonxsb->setStyleSheet("background-color: yellow;");
        boutonxsb->setFont(police);

        layout->addWidget(boutonxsb, i % MAX_COL, i / MAX_COL);

        // Connections
        connect(boutonxsb, SIGNAL(clicked()), this, SLOT(close()));
        connect(boutonxsb, &QPushButton::clicked, [niv] () {
            FenetreNiveau* c  = new FenetreNiveau(niv.carte());
            c->showMaximized();
        });

        i++;
    }

    setLayout(layout);
}


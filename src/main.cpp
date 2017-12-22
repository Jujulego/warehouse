// Importations
#include <functional>
#include <iomanip>
#include <iostream>

#include "outils/manip.hpp"
#include "outils/style.hpp"

#include "moteur/carte.hpp"

#include "affichage.hpp"
#include "fichierxsb.hpp"
#include "menu.hpp"
#include "niveau.hpp"

// Prototypes
void charger();

// Main !
#ifdef QTCREATOR

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QString>

#include "qt/fenetremenu.h"
#include "qt/fenetreniveau.h"

int main(int argc, char* argv[]) {
    // Initialiation de Qt
    QApplication app(argc, argv);

    // Traduction
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;

    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);

    // Petite fenetre
    FenetreMenu fen;
    fen.show();



    return app.exec();
}

#else
int main() {
	std::cout << std::boolalpha;
	std::cout << std::setprecision(2);

	Menu menu;

	Niveau niv("carte.txt");
	menu.ajouter("Jouer",          std::bind(&Niveau::jouer, niv));
	menu.ajouter("Charger (.xsb)", &charger);

	menu.afficher();

	return 0;
}
#endif

void charger() {
	// Nettoyage de l'ecran
	std::cout << manip::clear;
	afficher_entete(0, 0);

	// Lecture du nom de fichier
	std::string fichier;
	std::cout << manip::coord(0, 9) << "Entrez le nom du fichier : "; std::cin >> fichier;

	// Lecture du fichier
	FichierXSB fxsb(fichier);
	fxsb.as_menu().afficher();
}

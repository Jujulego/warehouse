// Importations
#include <functional>
#include <iostream>

#include "outils/manip.hpp"
#include "outils/style.hpp"

#include "carte.hpp"
#include "fichierxsb.hpp"
#include "menu.hpp"
#include "niveau.hpp"

// Prototypes
void charger();

// Main !
int main() {
	Menu menu;
	
	Niveau niv("carte.txt");
	menu.ajouter("Jouer",          std::bind(&Niveau::jouer, niv));
	menu.ajouter("Charger (.xsb)", &charger);
	
	menu.afficher();
	
	return 0;
}

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

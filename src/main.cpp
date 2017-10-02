// Importations
#include <functional>
#include <iostream>

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
	// Lecture du nom de fichier
	std::string fichier;
	std::cout << "Entrez le nom du fichier : "; std::cin >> fichier;
	
	// Lecture du fichier
	FichierXSB fxsb(fichier);
	fxsb.as_menu().afficher();
}

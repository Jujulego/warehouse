// Importations
#include <iostream>
#include <string>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/poussable.hpp"

#include "outils.hpp"
#include "outils/console.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"
#include "outils/style.hpp"

#include "carte.hpp"

// Main !
int main() {
	// Init
	Console console;
	
	moteur::Carte carte = moteur::Carte::charger("carte.txt");
	auto pers = std::make_shared<moteur::Deplacable>(&carte, 5);
	carte.set(0, 0, pers);
	
	// Objets
	carte.set(2, 2, std::make_shared<moteur::Poussable>(&carte, 1));
	carte.set(3, 3, std::make_shared<moteur::Poussable>(&carte, 2));
	carte.set(4, 4, std::make_shared<moteur::Poussable>(&carte, 3));
	carte.set(5, 5, std::make_shared<moteur::Poussable>(&carte, 4));
	carte.set(6, 6, std::make_shared<moteur::Poussable>(&carte, 5));
	carte.set(7, 7, std::make_shared<moteur::Poussable>(&carte, 6));
	
	std::cout << manip::clear;
	
	// Streams
	posstream<std::ostream> infos(  &std::cout, 6 + carte.taille_x() * 2, 6);
	posstream<std::ostream> erreurs(&std::cout, 6 + carte.taille_x() * 2, 4);
	erreurs.style(style::erreur);
	
	// Informations
	infos << "Force : " << pers->force();
	
	bool fin = false;
	while (!fin) {
		afficher_carte(carte, 2, 1);
		Coord dir(0, 0);
		
		// Touche
		switch (console.getch()) {
		case FL_HAUT:
			dir = HAUT;
			break;
		
		case FL_BAS:
			dir = BAS;
			break;
		
		case FL_GAUCHE:
			dir = GAUCHE;
			break;
		
		case FL_DROITE:
			dir = DROITE;
			break;
		
		case 'q':
			fin = true;
			break;
		}
		
		// Effacement des erreurs précedantes
		erreurs << manip::eff_ligne;
		
		// Action !
		if ((dir != Coord(0, 0)) && pers->deplacer(dir)) {
			erreurs << manip::buzz << style::souligne << "Mouvement impossible !";
			erreurs.flush();
		}
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;

	return 0;
}

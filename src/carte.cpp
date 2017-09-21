// Importations
#include <iostream>

#include "moteur/carte.hpp"
#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"

#include "carte.hpp"

// Prototype
void afficher_carte(moteur::Carte const& carte, int x, int y) {
	// Initialisation
	auto ref = manip::coord(x, y);
	
	// Cadre :
	std::cout << ref << TAB_DB;
	for (int i = 0; i < carte.taille_x(); ++i) {
		std::cout << TAB_DG;
	}
	std::cout << TAB_BG << std::endl;
	
	for (int i = 0; i < carte.taille_y(); ++i) {
		std::cout << manip::dx * x << TAB_HB << manip::dx * carte.taille_x() << TAB_HB << std::endl;
	}
	
	std::cout << manip::dx * x << TAB_HD;
	for (int i = 0; i < carte.taille_x(); ++i) {
		std::cout << TAB_DG;
	}
	std::cout << TAB_GH << std::endl;
}

void afficher_carte(moteur::Carte const& carte, Coord const& c) {
	afficher_carte(carte, c.x(), c.y());
}

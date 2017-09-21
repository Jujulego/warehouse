// Importations
#include <iostream>
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/obstacle.hpp"
#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"

#include "carte.hpp"

#ifdef __gnu_linux__
# define MUR "\xe2\x96\x88"
#else
# define MUR "#"
#endif

// Prototype
void afficher_carte(moteur::Carte const& carte, int x, int y) {
	// Initialisation
	auto ref = manip::coord(x, y);
	
	// Cadre :
#ifdef __gnu_linux__
	std::cout << ref << "\xe2\x96\x97";
	for (int i = 0; i < carte.taille_x(); ++i) {
		std::cout << "\xe2\x96\x84";
	}
	std::cout << "\xe2\x96\x96" << std::endl;
	
	for (int i = 0; i < carte.taille_y(); ++i) {
		std::cout << manip::dx * x << "\xe2\x96\x90" << manip::dx * carte.taille_x() << "\xe2\x96\x8c" << std::endl;
	}
	
	std::cout << manip::dx * x << "\xe2\x96\x9d";
	for (int i = 0; i < carte.taille_x(); ++i) {
		std::cout << "\xe2\x96\x80";
	}
	std::cout << "\xe2\x96\x98" << std::endl;
#else
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
#endif
	
	// Affichage des objets
	ref += Coord(1, 1);
	for (auto obj : carte) {
		std::cout << ref + obj->coord();
		
		if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
			std::cout << MUR;
		}
	}
}

void afficher_carte(moteur::Carte const& carte, Coord const& c) {
	afficher_carte(carte, c.x(), c.y());
}

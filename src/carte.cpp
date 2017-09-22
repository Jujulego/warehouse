// Importations
#include <iostream>
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/obstacle.hpp"
#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"
#include "outils/style.hpp"

#include "carte.hpp"

#ifdef __gnu_linux__
# define MUR  "\xe2\x96\x88"
# define PERS "P"
#else
# define MUR  "#"
# define PERS "P"
#endif

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
	
	// Affichage des objets
	ref += Coord(1, 1);
	for (auto obj : carte) {
		std::cout << ref + obj->coord();
		
		auto dobj = obj->get();
		
		if (dobj) {
			std::cout << style::vert << PERS << style::defaut;
		} else {
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << MUR;
			} else {
				std::cout << " ";
			}
		}
	}
}

void afficher_carte(moteur::Carte const& carte, Coord const& c) {
	afficher_carte(carte, c.x(), c.y());
}

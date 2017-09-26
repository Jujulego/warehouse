// Importations
#include <iostream>
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/sortie.hpp"

#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"
#include "outils/style.hpp"

#include "carte.hpp"

#ifdef __gnu_linux__
# define MUR    "\xe2\x96\x88\xe2\x96\x88"
# define PERS   "\xe2\x98\xba "
# define BOITE  "\xe2\x9a"
# define EMPL   "\xe2\x9b\xb6 "
# define SORTIE "S "
#else
# define MUR    "\xdb\xdb"
# define PERS   ";)"
# define BOITE  "#"
# define EMPL   "[]"
# define SORTIE "S "
#endif

// Prototype
void afficher_carte(moteur::Carte const& carte, int x, int y) {
	// Initialisation
	auto ref = manip::coord(x, y);
	
	// Cadre :
	std::cout << ref << TAB_DB;
	for (int i = 0; i < carte.taille_x(); ++i) {
		std::cout << TAB_DG << TAB_DG;
	}
	std::cout << TAB_BG << std::endl;
	
	for (int i = 0; i < carte.taille_y(); ++i) {
		std::cout << manip::dx * x << TAB_HB << manip::dx * carte.taille_x() * 2 << TAB_HB << std::endl;
	}
	
	std::cout << manip::dx * x << TAB_HD;
	for (int i = 0; i < carte.taille_x(); ++i) {
		std::cout << TAB_DG << TAB_DG;
	}
	std::cout << TAB_GH << std::endl;
	
	// Affichage des objets
	ref += Coord(1, 1);
	for (auto obj : carte) {
		std::cout << ref + obj->coord() + manip::x * obj->coord().x();
		auto dobj = obj->get();
		
		if (dobj) {
			auto pobj = std::dynamic_pointer_cast<moteur::Poussable>(dobj);
			if (pobj) {
				#ifdef __gnu_linux__
				if (pobj->poids() <= 6) {
					std::string s = BOITE;
					s.append(1, '\x7f' + pobj->poids());
					
					std::cout << style::jaune << s << ' ' << style::defaut;
				} else {
					std::cout << style::jaune << "#" << pobj->poids() << style::defaut;
				}
				#else
				std::cout << style::jaune << BOITE << pobj->poids() << style::defaut;
				#endif
			} else {
				std::cout << style::vert << PERS << style::defaut;
			}
		} else {
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << MUR;
			} else if (std::dynamic_pointer_cast<moteur::Emplacement>(obj)) {
				std::cout << EMPL;
			} else if (std::dynamic_pointer_cast<moteur::Sortie>(obj)) {
				std::cout << SORTIE;
			} else {
				std::cout << "  ";
			}
		}
	}
}

void afficher_carte(moteur::Carte const& carte, Coord const& c) {
	afficher_carte(carte, c.x(), c.y());
}

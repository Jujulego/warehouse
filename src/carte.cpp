// Importations
#include <array>
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
# define MUR    "\xe2\x97\x8b "
# define PERS   "\xe2\x98\xba "
# define BOITE  "\xe2\x9a"
# define EMPL   "\xe2\x9b\xb6 "
# define SORTIE style::fvert << "  " << style::defaut
#else
# define MUR    "\xde\xdd"
# define PERS   ";)"
# define BOITE  "#"
# define EMPL   "[]"
# define SORTIE style::fvert << "  " << style::defaut
#endif

// Fonctions
std::string select_mur(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Obstacle> const& mur) {
	static const std::array<std::string,16> murs = {
		MUR,                 // ----
		" "      + TAB_DG,   // ---D
		TAB_DB   + TAB_BG,   // --B-
		TAB_DB   + TAB_DBG,  // --BD
		TAB_DG   + " ",      // -G--
		TAB_DG   + TAB_DG,   // -G-D
		TAB_DBG  + TAB_BG,   // -GB-
		TAB_DBG  + TAB_DBG,  // -GBD
		TAB_HD   + TAB_GH,   // H---
		TAB_HD   + TAB_GHD,  // H--D
		TAB_HB   + TAB_HB,   // H-B-
		TAB_HB   + TAB_HDB,  // H-BD
		TAB_GHD  + TAB_GH,   // HG--
		TAB_GHD  + TAB_GHD,  // HG-D
		TAB_BGH  + TAB_HB,   // HGB-
		TAB_HDBG + TAB_HDBG, // HGBD
	};
	
	// Selection
	int cas = 0;
	
	if (carte->get<moteur::Obstacle>(mur->coord() + HAUT))   cas |= 0b1000;
	if (carte->get<moteur::Obstacle>(mur->coord() + BAS))    cas |= 0b0010;
	if (carte->get<moteur::Obstacle>(mur->coord() + GAUCHE)) cas |= 0b0100;
	if (carte->get<moteur::Obstacle>(mur->coord() + DROITE)) cas |= 0b0001;
	
	return murs[cas];
}

void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, int x, int y) {
	// Initialisation
	auto ref = manip::coord(x, y);
	
	// Affichage des objets
	ref += Coord(1, 1);
	for (auto obj : *carte) {
		std::cout << ref + obj->coord() + manip::x * obj->coord().x();
		auto dobj = obj->get();
		
		if (dobj) {
			auto pobj = std::dynamic_pointer_cast<moteur::Poussable>(dobj);
			
			if (pobj) {
				if (carte->get<moteur::Emplacement>(pobj->coord())) {
					std::cout << style::vert;
				} else {
					std::cout << style::jaune;
				}
				
				#ifdef __gnu_linux__
				if (pobj->poids() <= 6) {
					std::string s = BOITE;
					s.append(1, '\x7f' + pobj->poids());
					
					std::cout << s << ' ' << style::defaut;
				} else {
					std::cout << "#" << pobj->poids() << style::defaut;
				}
				#else
				std::cout << BOITE << pobj->poids() << style::defaut;
				#endif
			} else {
				std::cout << style::vert << PERS << style::defaut;
			}
		} else {
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << select_mur(carte, std::dynamic_pointer_cast<moteur::Obstacle>(obj));
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

void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, Coord const& c) {
	afficher_carte(carte, c.x(), c.y());
}

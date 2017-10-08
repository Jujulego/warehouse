// Importations
#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

#include "ia/solveur2.hpp"

#include "moteur/carte.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/sortie.hpp"

#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"
#include "outils/style.hpp"

#include "carte.hpp"

#ifdef __gnu_linux__
# define MUR    "\xe2\x97\x8b "
# define PERS   "\xe2\x98\xba "
# define BOITE  "\xe2\x9a"
# define EMPL   "\xe2\x9b\xb6 "
# define SORTIE style::fvert << "  " << style::defaut
#else
# define MUR    "\xc4\xc4"
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

void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, int x, int y, ia::Solveur2 const* solv2) {
	// Initialisation
	std::hash<Coord> hash(carte->taille_y());
	auto ref = manip::coord(x, y);
	
	// Aide :
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	std::list<ia::Solveur2::Poussee> poussees;
	std::vector<bool> zone;
	
	if (solv2) {
		zone     = solv2->zone_accessible(carte, carte->personnage()->coord());
		poussees = solv2->recup_poussees( carte, carte->personnage()->coord());
	}
	
	// Affichage des objets
	ref += Coord(1, 1);
	
	for (auto obj : *carte) {
		std::cout << ref + obj->coord() + manip::x * obj->coord().x();
		auto dobj = obj->get();
		Style st;
		
		// Fond coloré pour les case à partir desquelles une poussée peut être faite
		if (solv2 && std::find_if(
				poussees.begin(), poussees.end(),
				[&] (ia::Solveur2::Poussee const& p) -> bool { return p.pos == obj->coord(); }
			) != poussees.end()) {
		
			st.fnd(style::BLEU);
		}
		
		if (dobj) {
			auto pobj = std::dynamic_pointer_cast<moteur::Poussable>(dobj);
			
			if (pobj) {
				if (carte->get<moteur::Emplacement>(pobj->coord())) {
					st.txt(style::VERT);
				} else if (solv2 && solv2->deadlock(carte, pobj, pers->coord(), pers->force())) {
					st.txt(style::ROUGE);
				} else {
					st.txt(style::JAUNE);
				}
				
				#ifdef __gnu_linux__
				if (pobj->poids() <= 6) {
					std::string s = BOITE;
					s.append(1, '\x7f' + pobj->poids());

					std::cout << st << s << ' ' << style::defaut;
				} else {
					std::cout << st << "#" << pobj->poids() << style::defaut;
				}
				#else
				std::cout << st << BOITE << pobj->poids() << style::defaut;
				#endif
			} else {
				st.txt(style::VERT);
				std::cout << st << PERS << style::defaut;
			}
		} else {
			// Un peu de couleur !
			if (solv2 && zone[hash(obj->coord())]) {
				if (st.fnd() != style::BLEU) {
					st.txt(style::CYAN);
				}
			}
			
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << st << select_mur(carte, std::dynamic_pointer_cast<moteur::Obstacle>(obj));
			} else if (std::dynamic_pointer_cast<moteur::Emplacement>(obj)) {
				st.txt(style::DEFAUT_TEXTE);
				std::cout << st << EMPL;
			} else if (std::dynamic_pointer_cast<moteur::Sortie>(obj)) {
				std::cout << st << SORTIE;
			} else if (solv2 && zone[hash(obj->coord())]) {
				std::cout << st << "--";
			} else {
				std::cout << st << "  ";
			}
		}
	}
	
	// Reset style
	std::cout << style::defaut;
}

void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, Coord const& c, ia::Solveur2 const* solv2) {
	afficher_carte(carte, c.x(), c.y(), solv2);
}

void afficher_entete(int x, int y) {
	afficher_entete(Coord(x, y));
}

void afficher_entete(Coord const& c) {
	// Entete
	posstream<std::ostream> stream(&std::cout, c);
	
	stream << "__          __                      __   __" << std::endl;
	stream << "\\ \\        / /                     / /  / /" << std::endl;
	stream << " \\ \\  /\\  / / ___    ____  ___    / /__/ / ___    __  __  _____  ___" << std::endl;
	stream << "  \\ \\/  \\/ / ___ \\  / __/ / _ \\  / ___  / / _ \\  / / / / / ___/ / _ \\" << std::endl;
	stream << "   \\  /\\  / / _  / / /   /  __/ / /  / / / // / / /_/ / /__  / /  __/" << std::endl;
	stream << "    \\/  \\/  \\___/ /_/    \\___/ /_/  /_/  \\___/  \\____/ /____/  \\___/" << std::endl;
}

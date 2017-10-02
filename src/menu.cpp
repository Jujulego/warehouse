// Importations
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/console.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"
#include "outils/style.hpp"

#include "carte.hpp"
#include "menu.hpp"

// Paramètres
#define NB_LIGNES 15
#define MARGE     5
#define LARGEUR   70

// Méthodes
void Menu::ajouter(std::string const& nom, std::function<void()> const& fonc) {
	auto it = m_entrees.find(nom);
	
	if (it != m_entrees.end()) {
		it->second.fonc = fonc;
	} else {
		m_entrees[nom] = Entree {fonc, m_entrees.size()};
	}
}

void Menu::supprimer(std::string const& nom) {
	m_entrees.erase(nom);
}

void Menu::afficher() const {
	// Initialisation
	std::vector<std::string> entrees(m_entrees.size());
	unsigned item = 0, larg = 0;
	
	for (auto p : m_entrees) {
		entrees[p.second.ordre] = p.first;
		larg = max(larg, p.first.size());
	}
	
	entrees.push_back("Quitter");
	
	auto calcul_pos = [&] (int i) -> auto { return manip::coord(i/NB_LIGNES * (larg + MARGE), i % NB_LIGNES); };
	
	larg += 2;
	posstream<std::ostream> stream(&std::cout, (LARGEUR - ((larg + MARGE) * ((entrees.size() / NB_LIGNES) + 1))) / 2, 9);
	auto maj_aff = [&] () -> void {
		std::cout << manip::clear << manip::coord(0, 11 + entrees.size());
		
		// Entete
		afficher_entete(ORIGINE);
		
		for (unsigned i = 0; i < entrees.size(); i++) {
			// "selection"
			if (i == item) stream << style::inverse;
			
			stream << calcul_pos(i) << "- " << entrees[i];
			
			// "deselection"
			if (i == item) stream << style::noninverse;
		}
	};
	
	// Affichage
	maj_aff();
	
	// Interactions
	bool fin = false;
	while (!fin) {
		// Clavier !
		switch (console::getch()) {
		case FL_BAS:
			stream << calcul_pos(item) << "- " << entrees[item];
			item = (item + 1) % entrees.size();
			stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;
			
			break;
		
		case FL_HAUT:
			stream << calcul_pos(item) << "- " << entrees[item];
			
			if (item == 0) item = entrees.size();
			item--;
			
			stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;
			
			break;
		
		case FL_DROITE:
			if (entrees.size() > NB_LIGNES) {
				stream << calcul_pos(item) << "- " << entrees[item];
				
				item += NB_LIGNES;
				if (item >= entrees.size()) item %= NB_LIGNES;
				
				stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;
			}
			
			break;
		
		case FL_GAUCHE:
			if (entrees.size() > NB_LIGNES) {
				stream << calcul_pos(item) << "- " << entrees[item];
				
				if (item < NB_LIGNES) {
					unsigned tmp = (entrees.size() / NB_LIGNES) * NB_LIGNES;
					
					if (item >= entrees.size() % NB_LIGNES) {
						tmp -= NB_LIGNES;
					}
					item += tmp;
				} else {
					item -= NB_LIGNES;
				}
				
				stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;
			}
			
			break;
		
		case ENTREE:
			if (item == entrees.size() -1) {
				fin = true;
			} else {
				m_entrees.at(entrees[item]).fonc();
				maj_aff();
			}
			
			break;
		}
	}
}

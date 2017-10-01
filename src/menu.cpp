// Importations
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/console.hpp"
#include "outils/manip.hpp"
#include "outils/style.hpp"

#include "menu.hpp"

// Méthodes
void Menu::ajouter(std::string const& nom, std::function<void()> const& fonc) {
	m_entrees[nom] = fonc;
}

void Menu::supprimer(std::string const& nom) {
	m_entrees.erase(nom);
}

void Menu::afficher(int x, int y) const {
	afficher(Coord(x, y));
}

void Menu::afficher(Coord const& c) const {
	// Initialisation
	std::vector<std::string> entrees;
	unsigned item = 0, larg = 0;
	Console console;
	
	for (auto p : m_entrees) {
		entrees.push_back(p.first);
		larg = max(larg, p.first.size());
	}
	
	entrees.push_back("Quitter");
	
	// Affichage
	std::cout << manip::clear;
	std::cout << manip::coord(c);
	
	for (unsigned i = 0; i < entrees.size(); i++) {
		// "selection"
		if (i == item) std::cout << style::inverse;
		
		std::cout << manip::sauve << "- " << entrees[i];
		std::cout << manip::restore << manip::dy;
		
		// "deselection"
		if (i == item) std::cout << style::noninverse;
	}
	
	std::cout << std::endl;
	std::cout << manip::sauve;
	
	// Interactions
	bool fin = false;
	while (!fin) {
		// Clavier !
		switch (console.getch()) {
		case FL_BAS:
			if (item != entrees.size() -1) {
				std::cout << manip::y*item + c << "- " << entrees[item];
				item++;
				std::cout << manip::y*item + c << style::inverse << "- " << entrees[item] << style::noninverse;
				std::cout << manip::restore;
			}
			
			break;
		
		case FL_HAUT:
			if (item != 0) {
				std::cout << manip::y*item + c << "- " << entrees[item];
				item--;
				std::cout << manip::y*item + c << style::inverse << "- " << entrees[item] << style::noninverse;
				std::cout << manip::restore;
			}
			
			break;
		
		case ENTREE:
			if (item == entrees.size() -1) {
				fin = true;
			} else {
				m_entrees.at(entrees[item])();
			}
			
			break;
		}
	}
}

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

#include "menu.hpp"

// Méthodes
void Menu::ajouter(std::string const& nom, std::function<void()> const& fonc) {
	m_entrees[nom] = fonc;
}

void Menu::supprimer(std::string const& nom) {
	m_entrees.erase(nom);
}

void Menu::afficher() const {
	// Initialisation
	std::vector<std::string> entrees;
	unsigned item = 0, larg = 0;
	Console console;
	
	for (auto p : m_entrees) {
		entrees.push_back(p.first);
		larg = max(larg, p.first.size());
	}
	
	entrees.push_back("Quitter");
	
	posstream<std::ostream> stream(&std::cout, 20, 9);
	auto maj_aff = [&] () -> void {
		std::cout << manip::clear << manip::coord(ORIGINE);
		
		// Entete
		std::cout << "__          __                      __   __" << std::endl;
		std::cout << "\\ \\        / /                     / /  / /" << std::endl;
		std::cout << " \\ \\  /\\  / / ___    ____  ___    / /__/ / ___    __  __  _____  ___" << std::endl;
		std::cout << "  \\ \\/  \\/ / ___ \\  / __/ / _ \\  / ___  / / _ \\  / / / / / ___/ / _ \\" << std::endl;
		std::cout << "   \\  /\\  / / _  / / /   /  __/ / /  / / / // / / /_/ / /__  / /  __/" << std::endl;
		std::cout << "    \\/  \\/  \\___/ /_/    \\___/ /_/  /_/  \\___/  \\____/ /____/  \\___/" << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		
		for (unsigned i = 0; i < entrees.size(); i++) {
			// "selection"
			if (i == item) stream << style::inverse;
			
			stream << manip::y*i << "- " << entrees[i];
			std::cout << std::endl;
			
			// "deselection"
			if (i == item) stream << style::noninverse;
		}
		
		std::cout << std::endl;
	};
	
	// Affichage
	maj_aff();
	
	// Interactions
	bool fin = false;
	while (!fin) {
		// Clavier !
		switch (console.getch()) {
		case FL_BAS:
			if (item != entrees.size() -1) {
				stream << manip::y * item << "- " << entrees[item];
				item++;
				stream << manip::y * item << style::inverse << "- " << entrees[item] << style::noninverse;
			}
			
			break;
		
		case FL_HAUT:
			if (item != 0) {
				stream << manip::y * item << "- " << entrees[item];
				item--;
				stream << manip::y * item << style::inverse << "- " << entrees[item] << style::noninverse;
			}
			
			break;
		
		case ENTREE:
			if (item == entrees.size() -1) {
				fin = true;
			} else {
				m_entrees.at(entrees[item])();
				maj_aff();
			}
			
			break;
		}
	}
}

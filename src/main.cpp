// Importations
#include <iostream>
#include <memory>
#include <string>

#include "moteur/carte.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/personnage.hpp"

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
	
	std::shared_ptr<moteur::Carte> carte     = moteur::Carte::charger("carte.txt");
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	
	// Check personnage
	if (!pers) {
		std::cout << style::erreur << "Pas de personnage sur la carte ..." << style::defaut << std::endl;
		return 0;
	}
	
	std::cout << manip::clear;
	
	// Streams
	posstream<std::ostream> infos(  &std::cout, 6 + carte->taille_x() * 2, 8);
	posstream<std::ostream> erreurs(&std::cout, 6 + carte->taille_x() * 2, 4);
	erreurs.style(style::erreur);
	
	// Informations
	std::cout << manip::coord(6 + carte->taille_x() * 2, 6) << "Force : " << pers->force();
	
	bool fin = false;
	while (!fin) {
		afficher_carte(carte, 2, 1);
		
		// Etat des emplacements
		int nb = 0, i = 0;
		for (auto obj : *carte) {
			auto pt = std::dynamic_pointer_cast<moteur::Emplacement>(obj);
			
			if (pt) {
				nb++;
				
				if (pt->a_bloc()) i++;
			}
		}
		
		infos << manip::eff_ligne << i << " / " << nb;
		infos.flush();
		
		// Touche
		Coord dir(0, 0);
		
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
		
		case 'r':
			carte = moteur::Carte::charger("carte.txt");
			pers  = carte->personnage();
			
			break;
		}
		
		// Effacement des erreurs précedantes
		erreurs << manip::eff_ligne;
		
		// Action !
		if ((dir != Coord(0, 0)) && pers->deplacer(dir)) {
			std::cout << manip::buzz;
			erreurs << style::souligne << "Mouvement impossible !";
			erreurs.flush();
		}
		
		// Test de fin
		if (carte->test_fin()) {
			afficher_carte(carte, 2, 1);
			infos << manip::eff_ligne << style::vert << "Bien joué !";
			break;
		}
	}
	
	std::cout << manip::coord(0, carte->taille_y() + 4);
	std::cout.flush();
	
	return 0;
}

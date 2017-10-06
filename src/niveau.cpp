// Importations
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "ia/chemin.hpp"
#include "ia/solveur.hpp"

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
#include "niveau.hpp"

// Namespaces
using namespace std::literals::chrono_literals;

// Constructeur
Niveau::Niveau(std::string const& fichier) : m_carte(moteur::Carte::charger(fichier)) {}
Niveau::Niveau(std::shared_ptr<moteur::Carte> const& carte) : m_carte(carte) {}

// Méthodes
bool Niveau::jouer() {
	// Init
	std::shared_ptr<moteur::Carte> carte     = this->carte();
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	
	// Check personnage
	if (!pers) {
		std::cout << style::erreur << "Pas de personnage sur la carte ..." << style::defaut << std::endl;
		return 0;
	}
	
	// Solveur
	ia::Solveur solveur(carte, pers);
	ia::Chemin chemin;
	
	// Informations
	std::cout << manip::clear;
	afficher_entete(ORIGINE);
	
	std::cout << manip::coord(8 + carte->taille_x() * 2, 9) << style::souligne << "Statistiques :" << style::nonsouligne;
	std::cout << manip::coord(9 + carte->taille_x() * 2, 11) << "Force :      " << pers->force();
	std::cout << manip::coord(9 + carte->taille_x() * 2, 12) << "Solution :   -";
	std::cout << manip::coord(9 + carte->taille_x() * 2, 13) << "Mouvements : -";
	
	if (!m_infos.empty()) {
		auto dep = manip::coord(5, max(12 + carte->taille_y(), 20));
		for (auto p : m_infos) {
			if (p.first != "Title") {
				std::cout << dep << style::gras << p.first << style::nongras << ": " << style::italique << p.second << style::nonitalique;
				dep += manip::y;
			} else {
				std::cout << style::souligne << manip::coord(2, 7) << p.second << " :" << style::nonsouligne;
			}
		}
	}
	
	// Streams
	posstream<std::ostream> mouvstream(&std::cout, 22 + carte->taille_x() * 2, 13);
	posstream<std::ostream> infos(     &std::cout, 9  + carte->taille_x() * 2, 16);
	posstream<std::ostream> erreurs(   &std::cout, 9  + carte->taille_x() * 2, 17);
	erreurs.style(style::erreur);
	
	// Boucle de jeu
	bool fin = false, victoire = false;
	unsigned nb_mouv = 0;
	
	while (!fin) {
		afficher_carte(carte, 4, 8);
		
		// Etat des emplacements
		int nb = 0, i = 0;
		for (auto empl : carte->liste<moteur::Emplacement>()) {
			nb++;
			
			if (empl->a_bloc())
				i++;
		}
		
		infos << manip::eff_ligne << i << " / " << nb;
		
		// Touche
		Coord dir(0, 0);
		
		if (chemin.longueur() != 0) {
			dir = chemin.pop();
			std::this_thread::sleep_for(250ms);
		
		} else {
			switch (console::getch()) {
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
			
			case 's':
				infos << manip::eff_ligne << style::jaune << "Calcul en cours ...";
				chemin = solveur.resoudre();
				infos << manip::eff_ligne << style::defaut;
				
				std::cout << manip::coord(22 + carte->taille_x() * 2, 12) << chemin.longueur();
				break;
			
			case 'r':
				carte   = this->carte();
				pers    = carte->personnage();
				solveur = ia::Solveur(carte, pers);
				
				nb_mouv = 0;
				mouvstream << manip::eff_ligne << nb_mouv;
				
				break;
			}
		}
		
		// Effacement des erreurs précedantes
		erreurs << manip::eff_ligne;
		
		// Action !
		if (dir != Coord(0, 0)) {
			if (pers->deplacer(dir)) {
				std::cout << manip::buzz;
				erreurs << style::souligne << "Mouvement impossible !";
				erreurs.flush();
			} else {
				mouvstream << manip::eff_ligne << ++nb_mouv;
			}
		}
		
		// Test de fin
		if (carte->test_fin()) {
			afficher_carte(carte, 4, 8);
			
			infos << manip::eff_ligne << style::vert << "Bien joué !" << std::endl;
			infos << std::endl;
			infos << style::defaut << "[ Appuyez sur ENTREE ]";
			while (console::getch() != ENTREE) {}
			
			victoire = true;
			
			break;
		}
	}
	
	std::cout << manip::coord(0, carte->taille_y() + 4);
	std::cout.flush();
	
	return victoire;
}

// Accesseurs
std::shared_ptr<moteur::Carte> Niveau::carte() const {
	return std::make_shared<moteur::Carte>(*m_carte); // Copie !
}

std::map<std::string,std::string>& Niveau::infos() {
	return m_infos;
}

std::map<std::string,std::string> const& Niveau::infos() const {
	return m_infos;
}

// Importations
#include <chrono>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "ia/chemin.hpp"
#include "ia/solveur.hpp"
#include "ia/solveur2.hpp"

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
	
	// Solveurs
	ia::Solveur  solveur( carte, pers);
	ia::Solveur2 solveur2(carte, pers);
	ia::IA*      ia = nullptr;
	ia::Chemin   chemin;
	
	bool help_mode = false;
	
	// Futurs
	std::future<ia::Chemin> fut_chemin;
	
	// Informations
	std::cout << manip::clear;
	afficher_entete(ORIGINE);
	
	int posx = 9 + carte->taille_x() * 2, posy = 9;
	std::cout << manip::coord(posx -1, posy) << style::souligne << "Statistiques :" << style::nonsouligne;
	posy += 2;
	std::cout << manip::coord(posx, posy++) << "Force :      " << pers->force();
	std::cout << manip::coord(posx, posy++) << "Solution :   -";
	std::cout << manip::coord(posx, posy)   << "Mouvements : -";
	
	// Streams
	posstream<std::ostream> mouvstream(&std::cout, posx +13, posy++);
	posstream<std::ostream> iastats(   &std::cout, posx, 15);
	posstream<std::ostream> erreurs(   &std::cout, posx, 16);
	posstream<std::ostream> infos(     &std::cout, posx, 17);
	erreurs.style(style::erreur);
	
	// Commandes
	if (posx < 30) {
		posy = 9;
		posx += 20;
	} else {
		posy = 20;
	}
	
	std::cout << manip::coord(posx -1, posy++) << style::souligne << "Commandes :" << style::nonsouligne;
	std::cout << manip::coord(posx,    posy++) << " - " << style::gras << "R" << style::nongras << " : recommencer";
	std::cout << manip::coord(posx,    posy++) << " - " << style::gras << "H" << style::nongras << " : un peu d'aide ?";
	std::cout << manip::coord(posx,    posy++) << " - " << style::gras << "S" << style::nongras << " : calcul d'une solution";
	std::cout << manip::coord(posx,    posy++) << " - " << style::gras << "Q" << style::nongras << " : quitter";
	
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
	
	// Boucle de jeu
	bool fin = false, victoire = false;
	unsigned nb_mouv = 0;
	
	while (!fin) {
		// Affichage
		afficher_carte(carte, 4, 8, help_mode ? &solveur2 : nullptr);
		
		// Etat des emplacements
		int nb = 0, i = 0;
		for (auto empl : carte->liste<moteur::Emplacement>()) {
			nb++;
			
			if (empl->a_bloc())
				i++;
		}
		
		infos << manip::eff_ligne << i << " / " << nb;
		
		if (help_mode) {
			unsigned h = solveur2.heuristique(carte, carte->personnage()->force());
			
			#ifdef __gnu_linux__
			infos << " " << (h == std::numeric_limits<unsigned>::max() ? "\xe2\x88\x9e" : to_string(h));
			#else
			infos << " " << (h == std::numeric_limits<unsigned>::max() ? "inf" : to_string(h));
			#endif
		}
		
		// Check future
		if (fut_chemin.valid() && fut_chemin.wait_for(100ms) == std::future_status::ready) {
			chemin = fut_chemin.get();
		}
		
		// Touche
		Coord dir(0, 0);
		
		if (chemin.longueur() != 0) {
			dir = chemin.pop();
			std::this_thread::sleep_for(250ms);
		
		} else {
			switch (console::getch(!fut_chemin.valid())) {
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
			
			case 'i':
				if (fut_chemin.valid()) {
					ia->interrompre();
				}
				
				break;
			
			case 'h':
				help_mode = !help_mode;
				break;
			
			case 's':
				// Ignoré si calcul déjà en cours
				if (fut_chemin.valid()) break;
				
				// Choix de l'IA
				do {
					int num = 0;
					
					infos << manip::eff_ligne << "Quelle IA (1 ou 2) ? ";
					std::cout << infos.coord() + manip::y;
					std::cin >> num;
					
					switch (num) {
						case 1:
							ia = &solveur;
							break;
						
						case 2:
							ia = &solveur2;
							break;
					}
				} while(!ia);
				
				// Execution
				infos << manip::eff_ligne << style::jaune << "Calcul en cours ...";
				fut_chemin = ia->async_resoudre(iastats);
				infos << manip::eff_ligne << style::defaut;
				
				std::cout << manip::coord(22 + carte->taille_x() * 2, 12) << chemin.longueur();
				break;
			
			case 'r':
				carte   = this->carte();
				pers    = carte->personnage();
				solveur  = ia::Solveur( carte, pers);
				solveur2 = ia::Solveur2(carte, pers);
				
				nb_mouv = 0;
				mouvstream << manip::eff_ligne << nb_mouv;
				
				break;
			}
		}
		
		// Effacement des erreurs précedantes
		erreurs << manip::eff_ligne;
		
		// Action !
		if (!fut_chemin.valid() && dir != Coord(0, 0)) {
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
			
			#ifdef __gnu_linux__
			infos << manip::eff_ligne << style::vert << "Bien joué !" << std::endl;
			#else
			infos << manip::eff_ligne << style::vert << "Bien joue !" << std::endl;
			#endif
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

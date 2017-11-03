// Importations
#include <iostream>
#include <memory>

#include "outils/console.hpp"
#include "outils/manip.hpp"

#include "moteur/carte.hpp"

#include "affichage.hpp"
#include "devscreen.hpp"

// Constructeur
DevScreen::DevScreen(std::shared_ptr<moteur::Carte> carte) : m_carte(carte) {}

// Méthodes
void DevScreen::afficher() {
	// Entete
	std::cout << manip::clear;
	afficher_entete(ORIGINE);

	// Getch !
	bool continuer = true;
	while (continuer) {
		// Interaction !
		switch (console::getch()) {
		case 'q':
			continuer = false;
			break;
		}
	}
}

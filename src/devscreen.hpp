#pragma once

// Importations
#include <memory>

#include "moteur/carte.hpp"

// Classe
class DevScreen {
	public:
		// Constructeur
		DevScreen(std::shared_ptr<moteur::Carte> carte);

		// Méthodes
		void afficher();

	private:
		// Attributs
		std::shared_ptr<moteur::Carte> m_carte;
};

#pragma once

// Importations
#include <string>
#include <map>

#include "menu.hpp"
#include "niveau.hpp"

// Classe
class FichierXSB {
	public:
		// Constructeur
		FichierXSB(std::string nom);

		// Méthodes
		int nb_niv() const;
		Menu as_menu() const;

		std::vector<Niveau> const& niveaux() const;

	private:
		// Méthodes
		void charger(std::string const& fichier);

		// Attributs
		std::vector<Niveau> m_niveaux;
};

#pragma once

// Importations
#include <map>
#include <memory>

#include "moteur/carte.hpp"
#include "outils/posstream.hpp"

// Classe
class Niveau {
	public:
		// Constructeur
		Niveau(std::string const& fichier);
		Niveau(std::shared_ptr<moteur::Carte> const& carte);
		
		// Méthodes
		bool jouer(); // renvoie vrai en cas de victoire
		
		// Accesseurs
		std::shared_ptr<moteur::Carte> carte() const; // c'est une copie !
		
		std::map<std::string,std::string>& infos();
		std::map<std::string,std::string> const& infos() const;
	
	private:
		// Méthodes
		void init_aff();

		// Attributs
		std::shared_ptr<moteur::Carte> m_carte;
		std::map<std::string,std::string> m_infos;

		posstream<std::ostream> pushstream;
		posstream<std::ostream> mouvstream;
		posstream<std::ostream> infostream;
		posstream<std::ostream> iastats;
		posstream<std::ostream> erreurs;
};

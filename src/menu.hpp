#pragma once

// Importations
#include <functional>
#include <map>
#include <string>

#include "outils/coord.hpp"

// Classe
class Menu {
	public:
		// Méthodes
		void ajouter(std::string const& nom, std::function<void()> const& fonc);
		void supprimer(std::string const& nom);
		
		void afficher() const;
		
		// Templates
		template<class F>
		void ajouter(std::string const& nom, F const& fonc) {
			ajouter(nom, std::function<void()>(fonc));
		}
	
	private:
		// Attributs
		std::map<std::string,std::function<void()>> m_entrees;
};

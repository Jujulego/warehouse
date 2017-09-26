#pragma once

// Importations
#include <list>
#include <memory>

#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"

// Namespace
namespace ia {

// Classe
class Chemin {
	public:
		// Alias
		using iterator       = std::list<Coord>::iterator;
		using const_iterator = std::list<Coord>::const_iterator;
		
		// Méthodes
		void ajouter(int x, int y);
		void ajouter(Coord const& c);
		
		bool appliquer(std::shared_ptr<moteur::Deplacable> const& obj) const;
		
		// Itérateurs
		iterator begin();
		iterator end();
		
		const_iterator begin() const;
		const_iterator end() const;
	
	private:
		// Attributs
		std::list<Coord> m_actions;
};

} // ia

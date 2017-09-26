#pragma once

// Importations
#include <memory>

#include "outils/coord.hpp"

#include "chemin.hpp"

// Namespace
namespace ia {

// Classe
class Noeud {
	public:
		// Constructeur
		Noeud();
		Noeud(std::shared_ptr<Noeud> const& pere, Coord const& mvt);
		
		// Méthodes
		Chemin chemin() const;
		
		// Accesseur
		Coord mvt() const;
		std::shared_ptr<Noeud> pere() const;
	
	private:
		// Attributs
		std::shared_ptr<Noeud> m_pere;
		Coord m_mvt;
};

} // ia

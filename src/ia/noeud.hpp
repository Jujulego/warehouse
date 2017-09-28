#pragma once

// Importations
#include <memory>

#include "moteur/carte.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"

// Namespace
namespace ia {

// Classe
class Noeud {
	public:
		// Constructeur
		Noeud();
		Noeud(int x, int y, std::shared_ptr<Noeud> const& pere);
		Noeud(Coord const& mvt, std::shared_ptr<Noeud> const& pere);
		
		// Méthodes
		Chemin chemin() const;
		std::shared_ptr<moteur::Carte> carte(std::shared_ptr<moteur::Carte> const& base, Coord& obj, int force);
		
		// Accesseur
		Coord mvt() const;
		std::shared_ptr<Noeud> pere() const;
	
	private:
		// Attributs
		Coord m_mvt;
		std::shared_ptr<Noeud> m_pere;
};

} // ia

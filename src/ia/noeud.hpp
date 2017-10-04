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
		Noeud(Chemin const& chemin, std::shared_ptr<Noeud> const& pere);
		
		// Méthodes
		Chemin chemin_complet() const;
		std::shared_ptr<moteur::Carte> carte(std::shared_ptr<moteur::Carte> const& base, Coord& obj, int force);
		
		// Accesseur
		Chemin chemin() const;
		std::shared_ptr<Noeud> pere() const;
	
	private:
		// Attributs
		Chemin m_chemin;
		std::shared_ptr<Noeud> m_pere;
};

} // ia

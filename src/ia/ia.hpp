#pragma once

// Importations
#include <list>
#include <memory>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"

// Namespace
namespace ia {

// Classe
class IA {
	public:
		// Constructeur
		IA(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj);
		
		// Destructeur
		virtual ~IA() = default;
		
		// Méthodes
		virtual Chemin resoudre() = 0;
	
	protected:
		// Attributs
		std::shared_ptr<moteur::Carte> m_carte;
		std::shared_ptr<moteur::Deplacable> m_obj;
		
		// Outils
		std::vector<int> reduire(std::shared_ptr<moteur::Carte> const& c) const;
		std::list<Coord> mouvements(std::shared_ptr<moteur::Deplacable> const& obj) const;
		bool comparer(std::shared_ptr<moteur::Carte> const& c1, std::shared_ptr<moteur::Carte> const& c2) const;
		bool trouver_chemin(std::shared_ptr<moteur::Carte> const& carte, Coord const& dep, Coord const& arr, Chemin& res) const;
};

} // ia

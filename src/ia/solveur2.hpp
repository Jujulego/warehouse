#pragma once

// Importations
#include <list>
#include <memory>
#include <ostream>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"
#include "ia.hpp"

// Namespace
namespace ia {

// Classe
class Solveur2 : public IA {
	public:
		// Structure
		struct Poussee {
			// Attributs
			Coord pos;
			Coord dir;
		};
		
		// Constructeur
		Solveur2(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj);
		
		// Méthodes
		virtual Chemin resoudre(posstream<std::ostream>& stream) override;
		
		// Outils
		unsigned heuristique(std::shared_ptr<moteur::Carte> const& carte) const;
		std::list<Poussee> recup_poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;
		
		std::vector<bool> zone_interdite( std::shared_ptr<moteur::Carte> const& carte, Coord const& dep) const;
		std::vector<bool> zone_accessible(std::shared_ptr<moteur::Carte> const& carte, Coord const& obj) const;
};

} // ia

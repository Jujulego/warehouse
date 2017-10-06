#pragma once

// Importations
#include <list>
#include <memory>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"

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
		virtual Chemin resoudre() override;
		std::vector<bool> zone_accessible(std::shared_ptr<moteur::Carte> const& carte, Coord const& obj) const;
		std::list<Poussee> recup_poussees(std::shared_ptr<moteur::Carte> const& carte, Coord const& obj) const;
};

} // ia

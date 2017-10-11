#pragma once

// Importations
#include <list>
#include <map>
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
		// Structures
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
		std::list<Poussee> recup_poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj, Coord const& prec = Coord(-1, -1)) const;
		std::vector<bool> zone_accessible(std::shared_ptr<moteur::Carte> const& carte, Coord const& obj) const;
		
		// - analyse statique
		bool est_tunnel(std::shared_ptr<moteur::Carte> const& carte, Coord const& pos, Coord const& dir) const;
		std::vector<bool> zone_interdite(std::shared_ptr<moteur::Carte> const& carte, Coord const& dep) const;
		
		std::pair<Coord,int> min_dist_empl(std::shared_ptr<moteur::Carte> const& carte, Coord const& pos) const;
		std::vector<std::map<Coord,int>> dists_empls(std::shared_ptr<moteur::Carte> const& carte) const;
	
	private:
		// Cache
		mutable std::vector<std::pair<Coord,int>> c_min_dist_empl;
		mutable std::vector<std::map<Coord,int>>  c_dists_empls;
		mutable std::vector<bool> c_zone_interdite;
};

} // ia

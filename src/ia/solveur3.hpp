#pragma once

// Importations
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

// Prototype
unsigned char get_mask(Coord const& dir);

// Classe
class Solveur3 : public IA {
	public:
		// Structure
		struct Infos {
			// Attributs
			bool coin = false;
			bool tunnel = false;
			bool interieur = false;       // A l'intérieur des murs
			bool stone_reachable = false; // Atteignable par un poussable

			unsigned char directions = 0;            // Directions vers tous les emplacements
			std::map<Coord,int> distances;           // Distance aux emplacements
			std::map<Coord,unsigned char> empl_dirs; // Directions aux emplacements

			// Constructeur
			Infos(size_t factx) : distances(std::less<Coord>(factx)), empl_dirs(std::less<Coord>(factx)) {}

			// Methodes
			void ajouter(Coord const& empl, Coord const& dir);

			bool test(Coord const& dir) const;
			bool test(Coord const& empl, Coord const& dir) const;
		};

		// Constructeur
		Solveur3(std::shared_ptr<moteur::Carte> carte, std::shared_ptr<moteur::Deplacable> obj);

		// Méthodes
		virtual Chemin resoudre(posstream<std::ostream>& stream) override;

		// Outils
		// - analyse statique
		std::vector<Infos> const& infos_cases() const;
		std::vector<bool> const& zone_interdite() const;
		std::vector<int> const& zones_empls() const;

		Infos const& infos_cases(Coord const& c) const;
		bool zone_interdite(Coord const& c) const;
		int const& zones_empls(Coord const& c) const;

		// - analyse dynamique
		std::vector<bool> zone_accessible(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;
		std::vector<unsigned char> poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;

	private:
		// Attributs
		std::hash<Coord> hash;

		// Cache
		mutable std::vector<Infos> c_infos;
		mutable std::vector<bool> c_zone_interdite;
		mutable std::vector<int> c_zones_empls;
};

} // ia

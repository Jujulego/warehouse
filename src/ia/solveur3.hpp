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

// Classe
class Solveur3 : public IA {
	public:
		// Structure
		struct Infos {
			// Attributs
			unsigned char directions = 0;  // Directions interressantes
			std::map<Coord,int> distances; // Distance aux emplacements

			// Constructeur
			Infos(size_t factx) : distances(std::less<Coord>(factx)) {}

			// Methodes
			void ajouter(Coord const& dir);
			bool test(Coord const& dir) const;
		};

		// Constructeur
		Solveur3(std::shared_ptr<moteur::Carte> carte, std::shared_ptr<moteur::Deplacable> obj);

		// Méthodes
		virtual Chemin resoudre(posstream<std::ostream>& stream) override;

		// - analyse statique
		std::vector<Infos> infos_cases(std::shared_ptr<moteur::Carte> carte) const;

	private:
		// Cache
		mutable std::vector<Infos> c_infos;
};

} // ia

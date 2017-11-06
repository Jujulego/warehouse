// Importations
#include <set>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"
#include "solveur3.hpp"

// Macros
#define MAJ_AFF 250

// Namespace
using namespace ia;

// Constructeur
Solveur3::Solveur3(std::shared_ptr<moteur::Carte> carte, std::shared_ptr<moteur::Deplacable> obj)
	: IA(carte, obj) {}

// Structure
void Solveur3::Infos::ajouter(Coord const& dir) {
	unsigned char mask;

	if      (dir == HAUT)   mask = 0b1000;
	else if (dir == BAS)    mask = 0b0100;
	else if (dir == GAUCHE) mask = 0b0010;
	else if (dir == DROITE) mask = 0b0001;

	directions |= mask;
}

bool Solveur3::Infos::test(Coord const& dir) const {
	unsigned char mask;

	if      (dir == HAUT)   mask = 0b1000;
	else if (dir == BAS)    mask = 0b0100;
	else if (dir == GAUCHE) mask = 0b0010;
	else if (dir == DROITE) mask = 0b0001;

	return directions & mask;
}

// Méthodes
Chemin Solveur3::resoudre(posstream<std::ostream>&) {
	return Chemin();
}

std::vector<Solveur3::Infos> Solveur3::infos_cases(std::shared_ptr<moteur::Carte> carte) const {
	// Cache
	if (c_infos.size() != 0) return c_infos;

	// Déclarations
	std::vector<Infos> infos(carte->taille_x() * carte->taille_y(), Infos(carte->taille_y()));
	std::hash<Coord> hash(carte->taille_y());

	// Parcours des emplacements
	for (auto empl : carte->liste<moteur::Emplacement>()) {
		// Init BFS
		std::queue<Coord> file;

		infos[hash(empl->coord())].distances[empl->coord()] = 0;
		file.push(empl->coord());

		// Algo !
		while (!file.empty()) {
			// Défilage
			Coord c = file.front();
			file.pop();

			// Evalutation des suivants !
			int nd = infos[hash(c)].distances[empl->coord()] + 1;

			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				// Les 2 précédantes cases doivent être des espaces (pas mur), pour pouvoir réaliser une poussée
				if (carte->get<moteur::Obstacle>(c - dir) || carte->get<moteur::Obstacle>(c - 2*dir)) {
					continue; // suivant !
				}

				// Marquage
				auto p = infos[hash(c - dir)].distances.emplace(empl->coord(), nd);
				if (!p.second) continue; // Déjà traité !

				// Ajout de la direction
				infos[hash(c - dir)].ajouter(dir);

				// Emfilage
				file.push(c - dir);
			}
		}
	}

	// Sauvegarde du résultat
	c_infos = infos;

	return infos;
}

// Importations
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <stack>
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

// Fonctions
unsigned char ia::get_mask(Coord const& dir) {
	if      (dir == HAUT)   return 0b1000;
	else if (dir == BAS)    return 0b0100;
	else if (dir == GAUCHE) return 0b0010;
	else if (dir == DROITE) return 0b0001;

	return 0;
}

// Constructeur
Solveur3::Solveur3(std::shared_ptr<moteur::Carte> carte, std::shared_ptr<moteur::Deplacable> obj)
	: IA(carte, obj), hash(carte->taille_y()) {}

// Structure
void Solveur3::Infos::ajouter(Coord const& empl, Coord const& dir) {
	directions      |= get_mask(dir);
	empl_dirs[empl] |= get_mask(dir);
}

bool Solveur3::Infos::test(Coord const& dir) const {
	return directions & get_mask(dir);
}

bool Solveur3::Infos::test(Coord const& empl, Coord const& dir) const {
	return empl_dirs.at(empl) & get_mask(dir);
}

// Méthodes
Chemin Solveur3::resoudre(posstream<std::ostream>&) {
	return Chemin();
}

std::vector<Solveur3::Infos> const& Solveur3::infos_cases() const {
	// Cache
	if (c_infos.size() != 0) return c_infos;

	// Création du tableau
	c_infos.resize(m_carte->taille_x() * m_carte->taille_y(), Infos(m_carte->taille_y()));

	// Parcours des emplacements
	for (auto empl : m_carte->liste<moteur::Emplacement>()) {
		// Init BFS
		std::queue<Coord> file;

		c_infos[hash(empl->coord())].distances[empl->coord()] = 0;
		file.push(empl->coord());

		// Algo !
		while (!file.empty()) {
			// Défilage
			Coord c = file.front();
			file.pop();

			// Evalutation des suivants !
			int nd = c_infos[hash(c)].distances[empl->coord()] + 1;

			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				Coord nc = c - dir;

				// Checks
				if (!m_carte->coord_valides(nc))        continue; // validité
				if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case est un espace libre

				// La case précédante est espace libre
				if (m_carte->get<moteur::Obstacle>(nc - dir)) continue;

				// Marquage
				auto p = c_infos[hash(nc)].distances.emplace(empl->coord(), nd);
				if (!p.second) {
					// Ajout de la direction
					if (p.first->second == nd) c_infos[hash(nc)].ajouter(empl->coord(), dir);

					continue; // Déjà traité !
				}

				// Ajout de la direction
				c_infos[hash(nc)].ajouter(empl->coord(), dir);

				// Enfilage
				file.push(nc);
			}
		}
	}

	// Parcours des poussables
	for (auto pous : m_carte->liste<moteur::Poussable>()) {
		if (c_infos[hash(pous->coord())].stone_reachable) continue;

		// Init BFS
		std::queue<Coord> file;

		c_infos[hash(pous->coord())].stone_reachable = true;
		file.push(pous->coord());

		// Algo !
		while (!file.empty()) {
			// Défilage
			Coord c = file.front();
			file.pop();

			// Evalutation des suivants !
			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				Coord nc = c + dir;

				// Checks
				if (!m_carte->coord_valides(nc))        continue; // validité
				if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case est un espace libre

				// La case précédante est espace libre
				if (m_carte->get<moteur::Obstacle>(c - dir)) continue;

				// Marquage
				if (c_infos[hash(nc)].stone_reachable) continue; // Déjà traité !
				c_infos[hash(nc)].stone_reachable = true;

				// Enfilage
				file.push(nc);
			}
		}
	}

	// Init DFS
	std::stack<Coord> pile;
	std::list<Coord> coins;

	Coord dep = m_carte->personnage()->coord();
	c_infos[hash(dep)].interieur = true;
	pile.push(dep);

	while (!pile.empty()) {
		// Dépilage
		Coord c = pile.top();
		pile.pop();

		// Test de 4 directions
		int nb_dirs = 0;
		unsigned char dirs = 0;
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			Coord nc = c + dir;

			// Checks
			if (!m_carte->coord_valides(nc))        continue; // validité
			if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case est un espace libre

			// Masque
			dirs |= get_mask(dir);
			++nb_dirs;

			// Marque ?
			if (c_infos[hash(nc)].interieur) continue;
			c_infos[hash(nc)].interieur = true;

			// Empilage
			pile.push(nc);
		}

		// Coin ou tunnel ?
		if (nb_dirs == 2) {
			if ((dirs & get_mask(HAUT)) && (dirs & get_mask(BAS))) {
				c_infos[hash(c)].coin   = false;
				c_infos[hash(c)].tunnel = true;
			} else if ((dirs & get_mask(GAUCHE)) && (dirs & get_mask(DROITE))) {
				c_infos[hash(c)].coin   = false;
				c_infos[hash(c)].tunnel = true;
			} else {
				c_infos[hash(c)].coin   = true;
				c_infos[hash(c)].tunnel = false;

				coins.push_back(c);
			}
		} else {
			c_infos[hash(c)].coin   = false;
			c_infos[hash(c)].tunnel = nb_dirs < 2;
		}
	}

	// Retour sur les coins
	for (auto c : coins) {
		// Si les 2 cases adjacentes sont des tunnels ou des coins => tunnel
		bool t = false;

		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			Coord nc = c + dir;

			// Checks
			if (!m_carte->coord_valides(nc))        continue; // validité
			if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case est un espace libre

			// Tunnel ?
			if (c_infos[hash(nc)].tunnel) {
				t = true;
				break;
			} else if (c_infos[hash(nc)].coin) {
				for (auto d : {HAUT, BAS, GAUCHE, DROITE}) {
					if (d == -dir) break;
					if (d ==  dir) break;

					// Checks
					if (!m_carte->coord_valides(nc + d)) continue;
					if (!m_carte->coord_valides(c  + d)) continue;

					// Test
					t |= !(m_carte->get<moteur::Obstacle>(nc + d) || m_carte->get<moteur::Obstacle>(c + d));
				}

				break;
			}
		}

		c_infos[hash(c)].tunnel = t;
	}

	return c_infos;
}

std::vector<bool> const& Solveur3::zone_interdite() const {
	// Cache
	if (c_zone_interdite.size() != 0) return c_zone_interdite;

	// Déduction depuis les infos
	std::vector<Infos> infos = infos_cases();
	c_zone_interdite.resize(infos.size(), false);

	for (unsigned i = 0; i < infos.size(); i++) {
		c_zone_interdite[i] = infos[i].interieur && infos[i].directions == 0b0000 && !m_carte->get<moteur::Emplacement>(hash.unhash(i));
	}

	return c_zone_interdite;
}

std::vector<int> const& Solveur3::zones_empls() const {
	// Cache
	if (c_zones_empls.size() != 0) return c_zones_empls;

	// Déduction depuis les infos
	std::vector<Infos> infos = infos_cases();
	c_zones_empls.resize(infos.size(), 0);

	// Parcours des emplacements
	int num = 1;
	for (auto empl : m_carte->liste<moteur::Emplacement>()) {
		if (c_zones_empls[hash(empl->coord())] != 0) continue;
		c_zones_empls[hash(empl->coord())] = num;

		// Init DFS
		std::stack<Coord> pile;
		pile.push(empl->coord());

		while (!pile.empty()) {
			// Dépilage
			Coord c = pile.top();
			pile.pop();

			// Suivants
			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				Coord nc = c + dir;

				// Checks
				if (!m_carte->coord_valides(nc))        continue; // Coordonnées invalides
				if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case est un espace libre
				if (infos[hash(nc)].tunnel && !m_carte->get<moteur::Emplacement>(nc)) continue; // La case est un tunnel mais pas un emplacement

				// Marque
				if (c_zones_empls[hash(nc)] != 0) continue;
				c_zones_empls[hash(nc)] = num;

				pile.push(nc);
			}
		}

		++num;
	}

	return c_zones_empls;
}

std::vector<bool> Solveur3::zone_accessible(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	// Déclarations
	std::vector<bool> zone(carte->taille_x() * carte->taille_y(), false);

	// Initialisation
	std::stack<Coord> pile;

	zone[hash(obj)] = true;
	pile.push(obj);

	// DFS !
	while (!pile.empty()) {
		// Depilage
		Coord c = pile.top();
		pile.pop();

		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Calcul coord suivantes
			Coord nc = c + dir;
			if (!carte->coord_valides(nc))   continue; // validité
			if (!(*carte)[nc]->accessible()) continue; // accessibilité
			if (zone[hash(nc)]) continue;              // marquage

			// Marquage et empilage
			zone[hash(nc)] = true;
			pile.push(nc);
		}
	}

	return zone;
}

Solveur3::Infos const& Solveur3::infos_cases(Coord const& c) const {
	return infos_cases()[hash(c)];
}

bool Solveur3::zone_interdite(Coord const& c) const {
	return zone_interdite()[hash(c)];
}

int const& Solveur3::zones_empls(Coord const& c) const {
	return zones_empls()[hash(c)];
}

std::vector<unsigned char> Solveur3::poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	// Détection des poussées en croisant les infos avec la zone accessible
	std::vector<unsigned char> poussees(carte->taille_x() * carte->taille_y(), 0);
	std::vector<bool>  zone  = zone_accessible(carte, obj);
	std::vector<Infos> infos = infos_cases();

	// Récupération du personnage
	std::shared_ptr<moteur::Personnage> pers = carte->get<moteur::Personnage>(obj);
	if (pers == nullptr) pers = carte->personnage();
	(*carte)[pers->coord()]->pop();

	// Parcours des poussables
	for (auto po : carte->liste<moteur::Poussable>()) {
		Coord c = po->coord();

		// Propositions
		unsigned char dirs = 0;

		for (auto p : infos[hash(c)].empl_dirs) {
			if (!carte->get<moteur::Emplacement>(p.first)->a_bloc()) dirs |= p.second;
		}

		// Test de directions
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Checks
			if (!(dirs & get_mask(dir))) continue; // Proposée
			if (!zone[hash(c - dir)])    continue; // Accessible
			if (carte->deplacer(po->coord(), dir, pers->force() - po->poids(), true)) continue;

			// Ajout !
			poussees[hash(c - dir)] |= get_mask(dir);
		}
	}

	carte->set(pers->coord(), pers);

	return poussees;
}

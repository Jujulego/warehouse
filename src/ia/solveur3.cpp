// Importations
#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <stack>
#include <vector>

#include <iostream>

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
	std::list<Coord> inter;

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
			c_infos[hash(c)].coin     = false;
			c_infos[hash(c)].culdesac = nb_dirs < 2;
			c_infos[hash(c)].tunnel   = nb_dirs < 2;
		}

		if (nb_dirs == 3) {
			inter.push_back(c);
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
					if (d == -dir) continue;
					if (d ==  dir) continue;

					// Checks
					if (!m_carte->coord_valides(nc + d)) continue;
					if (!m_carte->coord_valides(c  + d)) continue;

					// Test
					t |= (m_carte->get<moteur::Obstacle>(nc + d) == nullptr) ^ (m_carte->get<moteur::Obstacle>(c + d) == nullptr);
				}

				break;
			}
		}

		c_infos[hash(c)].tunnel = t;
	}

	// Retour sur les intersections
	std::vector<Coord> est_inter(m_carte->taille_x() * m_carte->taille_y(), ORIGINE);

	for (auto c : inter) {
		// tunnel si tunnel dans la direction opposée à l'obstacle ou tunnel sur les 2 autres côtés
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Checks
			if (m_carte->coord_valides(c + dir) && !m_carte->get<moteur::Obstacle>(c + dir)) continue; // Coordonnées valides et pas un obstacle
			est_inter[hash(c)] = dir;

			// Tunnel ?
			if (c_infos[hash(c - dir)].tunnel) {
				c_infos[hash(c)].tunnel = true;
				c_infos[hash(c)].porte  = false;

			} else if (dir == HAUT || dir == BAS) {
				c_infos[hash(c)].tunnel = c_infos[hash(c + DROITE)].tunnel && c_infos[hash(c + GAUCHE)].tunnel;

			} else if (dir == GAUCHE || dir == DROITE) {
				c_infos[hash(c)].tunnel = c_infos[hash(c + HAUT)].tunnel && c_infos[hash(c + BAS)].tunnel;
			}

			break; // Une seule direction vérifie le test
		}
	}

	// Détection des portes
	for (auto c : inter) {
		// Porte si 2 intersections voisines sont de directions opposées
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			Coord nc = c + dir;

			// Checks
			if (dir  == est_inter[hash(c)]) continue;     // direction interdite (obstacle ou coord invalide)
			if (-dir == est_inter[hash(c)]) continue;     // Uniquement sur les côtés
			if (est_inter[hash(nc)] == ORIGINE) continue; // pas une intersection !
			if (c_infos[hash(c)].tunnel && c_infos[hash(nc)].tunnel) continue; // Pas de porte au millieu d'un tunnel

			// Test
			c_infos[hash(c)].porte = (est_inter[hash(c)] == -est_inter[hash(nc)]);

			if (c_infos[hash(c)].porte) break;
		}
	}

	for (int h = 0; h < m_carte->taille_x() * m_carte->taille_y(); ++h) {
		Coord c = hash.unhash(h);

		// Check
		if (!c_infos[h].interieur) continue;

		for (auto dir : {HAUT, BAS, GAUCHE, HAUT}) {
			Coord nc = c + dir;

			// Checks
			if (!m_carte->coord_valides(nc)) continue;  // Coordonnées invalides
			if (!c_infos[hash(nc)].interieur) continue; // A l'interieur du labyrinthe

			// Porte ?
			c_infos[h].porte |= c_infos[h].tunnel ^ c_infos[hash(nc)].tunnel;
			c_infos[hash(nc)].porte |= c_infos[h].tunnel ^ c_infos[hash(nc)].tunnel;
		}
	}

	return c_infos;
}

std::vector<bool> const& Solveur3::zone_interdite() const {
	// Cache
	if (c_zone_interdite.size() != 0) return c_zone_interdite;

	// Déduction depuis les infos
	std::vector<Infos> const& infos = infos_cases();
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
	std::vector<Infos> const& infos = infos_cases();
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

				// Fusion de zones
				if (c_zones_empls[hash(nc)] != 0 && c_zones_empls[hash(nc)] != num) {
					int z1, z2;

					z1 = std::min(num, c_zones_empls[hash(nc)]);
					z2 = std::max(num, c_zones_empls[hash(nc)]);

					std::replace(c_zones_empls.begin(), c_zones_empls.end(), z2, z1);
					num = z1;

					continue;
				}

				// Checks
				if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case n'est pas un espace libre
				if (infos[hash(c)].porte && infos[hash(nc)].porte) continue; // On ne passe pas une porte
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

std::vector<int> const& Solveur3::ordre_empls() const {
	// Cache
	if (c_ordre_empls.size() != 0) return c_ordre_empls;

	// Déduction depuis les infos
	std::vector<Infos> const& infos = infos_cases();
	c_ordre_empls.resize(infos.size(), 0);

	// Parcours des emplacments
	std::list<std::shared_ptr<moteur::Emplacement>> empls = m_carte->liste<moteur::Emplacement>();
	std::queue<Coord> file;
	int prio_max = 0;

	// Dépendances & Priorités
//	std::cout << std::endl;
	std::vector<std::list<Coord>> priorites(infos.size());   // Emplacements devant etre rempli AVANT
	std::vector<std::list<Coord>> dependances(infos.size()); // Emplacements devant etre rempli APRES

	for (auto empl : empls) {
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			Coord nc = empl->coord() + dir;

			// Checks
			if (!m_carte->coord_valides(nc))            continue; // coordonnées valides
			if (!m_carte->get<moteur::Emplacement>(nc)) continue; // est un emplacement

			// Dépendant ? oui s'il est possible d'y accéder via dir
			if (infos[hash(nc)].directions & get_mask(-dir)) {
//				std::cout << (char)(empl->coord().x() + 'A') << empl->coord().y() << " < " << (char)(nc.x() + 'A') << nc.y() << std::endl;
				priorites[hash(nc)].push_back(empl->coord());
				dependances[hash(empl->coord())].push_back(nc);

				// La précedante doit etre vide egalement
				nc += dir;
				if (!m_carte->coord_valides(nc))            continue; // coordonnées valides
				if (!m_carte->get<moteur::Emplacement>(nc)) continue; // est un emplacement

//				std::cout << (char)(empl->coord().x() + 'A') << empl->coord().y() << " < " << (char)(nc.x() + 'A') << nc.y() << std::endl;
				priorites[hash(nc)].push_back(empl->coord());
				dependances[hash(empl->coord())].push_back(nc);
			}
		}
	}

	// Suppression des cycles simples : A -> B & B -> A sur les coins & cul-de-sacs
	for (auto empl : empls) {
		Coord a = empl->coord();
		if (!(infos[hash(a)].coin || infos[hash(a)].culdesac)) continue;

		for (auto ita = priorites[hash(a)].begin(); ita != priorites[hash(a)].end(); ++ita) {
			Coord b = *ita;

			auto itb = std::find(priorites[hash(b)].begin(), priorites[hash(b)].end(), a);
			if (itb == priorites[hash(b)].end()) continue;

			if (!infos[hash(b)].culdesac) {
				priorites[hash(a)].erase(ita--);
				dependances[hash(b)].remove(a);
			} else {
				priorites[hash(b)].erase(itb);
				dependances[hash(a)].remove(b);
			}
		}
	}

	// Pas de priorité : prio = 1
	for (auto it = empls.begin(); it != empls.end(); ++it) {
		Coord empl = (*it)->coord();

		// Dépendances ?
		if (priorites[hash(empl)].size() != 0) continue;

		// Application de la priorité
		c_ordre_empls[hash(empl)] = 1;
		prio_max = 1;

		file.push(empl);

		// Suppression de la liste
		empls.erase(it--);
	}

	// Outils
	enum R { CULDESAC = 0, COIN = 1, PLUS = 2 };
	auto est_coin = [this] (Coord const& c) -> R {
		int nb_hb = 0;
		int nb_dg = 0;

		for (auto dir : {HAUT, BAS}) {
			Coord nc = c + dir;

			if (!m_carte->coord_valides(nc))        continue; // coordonnées valides
			if (m_carte->get<moteur::Obstacle>(nc)) continue; // est un espace libre

			if (c_ordre_empls[hash(nc)] == 0) {
				++nb_hb;
			}
		}

		for (auto dir : {GAUCHE, DROITE}) {
			Coord nc = c + dir;

			if (!m_carte->coord_valides(nc))        continue; // coordonnées valides
			if (m_carte->get<moteur::Obstacle>(nc)) continue; // est un espace libre

			if (c_ordre_empls[hash(nc)] == 0) {
				++nb_dg;
			}
		}

		if (nb_hb == 1 && nb_dg == 1) return COIN;
		if (nb_hb + nb_dg == 1)       return CULDESAC;
		return PLUS;
	};

	while (!file.empty()) {
		// Défilage
		Coord empl = file.front();
		file.pop();

		// Enfilage des dépendances dont toutes les priorités sont définies
//		std::cout << (char)(empl.x() + 'A') << empl.y() << std::endl;
		for (auto dep : dependances[hash(empl)]) {
			if (c_ordre_empls[hash(dep)] != 0) continue;

//			std::cout << " " << (char)(dep.x() + 'A') << dep.y() << std::endl;
			bool ok = true;
			int p = 0;

			for (auto itd = priorites[hash(dep)].begin(); itd != priorites[hash(dep)].end(); ++itd) {
				Coord prio = *itd;
//				std::cout << "  " << (char)(prio.x() + 'A') << prio.y() << " " << c_ordre_empls[hash(prio)] << std::endl;

				if (c_ordre_empls[hash(prio)] == 0) {
					// Recherche d'un cycle simple
					auto itp = std::find(priorites[hash(prio)].begin(), priorites[hash(prio)].end(), dep);
					if (itp != priorites[hash(prio)].end()) { // Il y a un cycle !
						if (est_coin(dep) > est_coin(prio)) {
							ok = false;
							break;
						}
					} else {
						ok = false;
						break;
					}
				}

				p = std::max(p, c_ordre_empls[hash(prio)]);
			}

			if (ok) {
				c_ordre_empls[hash(dep)] = ++p;
				empls.remove_if([&dep] (std::shared_ptr<moteur::Emplacement> pt) { return pt->coord() == dep; });
				file.push(dep);
			}
		}
	}

	// Les autres sont interdépendants => meme priorité (max)
	prio_max = 99;
	for (auto empl : empls) {
		c_ordre_empls[hash(empl->coord())] = prio_max;
	}

	return c_ordre_empls;
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

int const& Solveur3::ordre_empls(Coord const& c) const {
	return ordre_empls()[hash(c)];
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

std::vector<unsigned char> Solveur3::poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	// Détection des poussées en croisant les infos avec la zone accessible
	std::vector<unsigned char> poussees(carte->taille_x() * carte->taille_y(), 0);
	std::vector<Infos> const& infos = infos_cases();
	std::vector<bool>         zone  = zone_accessible(carte, obj);

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
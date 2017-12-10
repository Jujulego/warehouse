// Importations
#include <algorithm>
#include <functional>
#include <limits>
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
	: IA(carte, obj) {}

// Structures
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

unsigned char Solveur3::Empl::dirs() const {
	unsigned char dirs = 0;

	for (auto p : suivants) {
		dirs |= get_mask((p.second - p.first) / 2);
	}

	return dirs;
}

// Méthodes
Chemin Solveur3::resoudre(posstream<std::ostream>&) {
	// Outils
	struct Etat {
	};

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

			// Il n'y a des intersections que dans les tunnels !
			c_infos[hash(c)].intersection = c_infos[hash(c)].tunnel;

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
			if (est_inter[hash(c)] == -est_inter[hash(nc)]) {
				c_infos[hash(c)].porte |= get_mask(dir);
			}
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
			if (c_infos[h].tunnel ^ c_infos[hash(nc)].tunnel) { // L'un des 2 est un tunnel (mais pas les 2 !)
				c_infos[h].porte        |= get_mask(dir);
				c_infos[hash(nc)].porte |= get_mask(-dir);

				// 4 portes = intersection
				c_infos[h].intersection        |= (c_infos[h].porte        == 0b1111);
				c_infos[hash(nc)].intersection |= (c_infos[hash(nc)].porte == 0b1111);
			}
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

std::vector<Solveur3::Empl> const& Solveur3::infos_empls() const {
	// Cache
	if (c_infos_empls.size() != 0) return c_infos_empls;

	// Déduction depuis les infos
	std::vector<Infos> const& infos = infos_cases();
	c_infos_empls.resize(infos.size(), Empl {0, {}});

	// Parcours des emplacements
	int num = 1;
	for (auto empl : m_carte->liste<moteur::Emplacement>()) {
		if (c_infos_empls[hash(empl->coord())].zone != 0) continue;
		c_infos_empls[hash(empl->coord())].zone = num;

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
				if (!m_carte->coord_valides(nc)) continue; // Coordonnées invalides

				// Fusion de zones
				if (c_infos_empls[hash(nc)].zone != 0 && c_infos_empls[hash(nc)].zone != num) {
					int z1, z2;

					z1 = std::min(num, c_infos_empls[hash(nc)].zone);
					z2 = std::max(num, c_infos_empls[hash(nc)].zone);

					for (Empl& e : c_infos_empls) {
						if (e.zone == z1) e.zone = z2;
					}
					num = z2;

					continue;
				}

				// Checks
				if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case n'est pas un espace libre
				if (infos[hash(c)].porte && infos[hash(nc)].porte) continue; // On ne passe pas une porte

				// Lien !
				Coord pc = c - dir;
				if (m_carte->coord_valides(pc) && !m_carte->get<moteur::Obstacle>(pc)) {
					c_infos_empls[hash(c)].suivants.push_back({pc, nc});
				}

				// Marque
				if (c_infos_empls[hash(nc)].zone != 0) continue;
				c_infos_empls[hash(nc)].zone = num;

				pile.push(nc);
			}
		}

		++num;
	}

	return c_infos_empls;
}

Solveur3::Infos const& Solveur3::infos_cases(Coord const& c) const {
	return infos_cases()[hash(c)];
}

bool Solveur3::zone_interdite(Coord const& c) const {
	return zone_interdite()[hash(c)];
}

Solveur3::Empl const& Solveur3::infos_empls(Coord const& c) const {
	return infos_empls()[hash(c)];
}

unsigned Solveur3::heuristique(std::shared_ptr<moteur::Carte> carte) const {
	return 0;
}

Coord Solveur3::choix_empl(std::shared_ptr<moteur::Carte> carte, std::list<Coord> empls) const {
	// Suppression de toutes les coordonées ne correspondant pas à un emplacement
	empls.erase(
		std::remove_if(empls.begin(), empls.end(),
			[&carte] (Coord const& empl) {
				return carte->get<moteur::Emplacement>(empl) == nullptr;
			}
		),
		empls.end()
	);

	// Pas de choix ...
	if (empls.size() == 0) return Coord(-1, -1);
	if (empls.size() == 1) return empls.front();

	// Suppression du personnage
	std::vector<Empl> infos = infos_empls();
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	(*carte)[pers->coord()]->pop();

	// Choix !
	int nb_prio = std::numeric_limits<int>::max();
	Coord choix = ORIGINE;

	for (auto empl : empls) {
		// Décompte des priorités
		std::list<Coord> marques;
		std::queue<Coord> file;
		int nb = -1;

		marques.push_back(empl);
		file.push(empl);

		// Algo
		while (!file.empty()) {
			Coord c = file.front();
			file.pop();

			// Décompte
			if (carte->get<moteur::Emplacement>(c)) {
				++nb;

				// Condition d'arrêt prématuré
				if (nb >= nb_prio) break;
			}

			// Suivants
			for (auto prio : infos[hash(empl)].suivants) {
				if (!(*carte)[prio.first]->accessible()) continue;
				if (!(*carte)[prio.second]->accessible()) continue;
				if (std::find(marques.begin(), marques.end(), prio.second) != marques.end()) continue;

				marques.push_back(prio.second);
				file.push(prio.second);
			}
		}

		// Choix !
		if (nb < nb_prio) {
			choix = empl;
			nb_prio = nb;
		}

		if (nb_prio == 0) break; // peu pas faire mieux !
	}

	carte->set(pers->coord(), pers);

	return choix;
}

std::vector<unsigned char> Solveur3::poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	// Détection des poussées en croisant les infos avec la zone accessible
	std::vector<unsigned char> poussees(carte->taille_x() * carte->taille_y(), 0);

	// Récupération du personnage
	std::shared_ptr<moteur::Personnage> pers = carte->get<moteur::Personnage>(obj);
	if (pers == nullptr) pers = carte->personnage();
	(*carte)[pers->coord()]->pop();

	// Parcours des poussables
	for (auto po : carte->liste<moteur::Poussable>()) {
		Coord c = po->coord();

		// Test de directions
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Checks
			if (!carte->coord_valides(c - dir)) continue;
			if (!(*carte)[c - dir]->accessible()) continue;
			if (carte->deplacer(po->coord(), dir, pers->force() - po->poids(), true)) continue;

			// Ajout !
			poussees[hash(c - dir)] |= get_mask(dir);
		}
	}

	carte->set(pers->coord(), pers);

	return poussees;
}

std::vector<bool> Solveur3::zone_interdite(std::shared_ptr<moteur::Carte> carte) const {
	// Base statique
	std::vector<Infos> const& infos = infos_cases();
	std::vector<bool> zone = zone_interdite();

	// Récupération du personnage
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	(*carte)[pers->coord()]->pop();

	// Zones interdites causées par les poussables
	std::queue<Coord> intersections;

	for (auto pous : carte->liste<moteur::Poussable>()) {
		Coord c = pous->coord();
		zone[hash(c)] = true;

		// Cas du poussable sur intersection
		if (infos[hash(c)].intersection) {
			intersections.push(c);
			continue;
		}

		// Tunnels : interdits jusqu'à sortie ou intersection
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Initialisation
			Coord ct = c + dir;
			if (!carte->coord_valides(ct)) continue;

			// Parcours du tunnel
			for (; carte->coord_valides(ct) && infos[hash(ct)].tunnel && infos[hash(ct)].interieur; ct += dir) {
				zone[hash(ct)] = true;

				if (infos[hash(ct)].intersection) {
					zone[hash(ct)] = false;
					intersections.push(ct);
					break;
				} else if (infos[hash(ct)].coin) {
					// on tourne au coins
					for (auto ndir : {HAUT, BAS, GAUCHE, DROITE}) {
						if ((ndir == dir) || (ndir == -dir)) continue;
						if (!carte->coord_valides(ct + ndir)) continue;

						if ((*carte)[ct + ndir]->accessible()) {
							dir = ndir;
							break;
						}
					}
				}
			}
		}
	}

	// Prolongement des intersections
	while (!intersections.empty()) {
		// Défilage
		Coord c = intersections.front();
		intersections.pop();

		// Tests
		int nb_voies = 0;
		int nb_inter = 0;
		std::vector<Coord> dir_inter;

		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			if (!carte->coord_valides(c + dir)) continue;

			// Décomptes
			if ((*carte)[c + dir]->accessible()) {
				++nb_voies;

				if (zone[hash(c + dir)]) {
					dir_inter.push_back(dir);
					++nb_inter;
				}
			}
		}

		// 2 voies pour 4 (ou 1 pour 3) : seule l'intersection est interdite
		if (nb_voies - nb_inter == 2) {
			if (nb_voies != 3) {
				zone[hash(c)] = true;
			} else {
				if ((*carte)[c - dir_inter[0]]->accessible()) {
					zone[hash(c)] = true;
				}
			}

		} else if (nb_inter != nb_voies && ((nb_voies == 2 && nb_inter == 1) || (nb_voies == 3 && nb_inter == 2) || (nb_voies == 4 &&  nb_inter >= 2))) {
			zone[hash(c)] = true;

			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				if (!carte->coord_valides(c + dir)) continue;

				// Interdiction de la voie restante
				if (!zone[hash(c + dir)]) {
					for (Coord ct = c + dir; carte->coord_valides(ct) && infos[hash(ct)].tunnel && infos[hash(ct)].interieur; ct += dir) {
						zone[hash(ct)] = true;

						if (infos[hash(ct)].intersection) {
							intersections.push(ct);
							break;
						} else if (infos[hash(ct)].coin) {
							// on tourne au coins
							for (auto ndir : {HAUT, BAS, GAUCHE, DROITE}) {
								if ((ndir == dir) || (ndir == -dir)) continue;
								if (!carte->coord_valides(ct + ndir)) continue;

								if ((*carte)[ct + ndir]->accessible()) {
									dir = ndir;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	carte->set(pers->coord(), pers);

	return zone;
}

std::vector<bool> Solveur3::zone_sr(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	// Initialisation
	std::vector<bool> resultat(carte->taille_x() * carte->taille_y(), false);

	// Extraction du poussable obj
	std::shared_ptr<moteur::Poussable> pous = carte->get<moteur::Poussable>(obj);
	if (pous == nullptr) return resultat;
	(*carte)[obj]->pop();

	// Extraction du personnage
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	(*carte)[pers->coord()]->pop();

	// Init BFS
	std::map<std::pair<Coord,Coord>,bool,std::function<bool(std::pair<Coord,Coord> const&,std::pair<Coord,Coord> const&)>> marques(
		[this] (std::pair<Coord,Coord> const& p1, std::pair<Coord,Coord> const& p2) {
			return (hash(p1.first) == hash(p2.first)) ? (hash(p1.second) < hash(p2.second)) : (hash(p1.first) < hash(p2.first));
		}
	);
	std::queue<std::pair<Coord,Coord>> file;
	marques[{obj, pers->coord()}] = true;
	resultat[hash(obj)] = true;
	file.push({obj, pers->coord()});

	// Algo !
	while (!file.empty()) {
		// Défilage
		Coord c = file.front().first;
		Coord p = file.front().second;
		file.pop();

		// Calcul zone accessible
		carte->set(c, std::make_shared<moteur::Poussable>(carte.get(), pous->poids()));
		std::vector<bool> z = zone_accessible(carte, p);
		(*carte)[c]->pop();

		// Evalutation des suivants !
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			Coord nc = c + dir;

			// La case précédante est accessible
			if (!m_carte->coord_valides(c - dir)) continue; // validité
			if (!z[hash(c - dir)])                continue; // accessibilité

			// Mouvement possible ?
			if (!m_carte->coord_valides(nc))   continue; // validité
			if (!(*m_carte)[nc]->accessible()) continue; // accessibilité

			// Marquage
			if (marques[{nc, c}]) continue; // Déjà traité !
			marques[{nc, c}] = true;
			resultat[hash(nc)] = true;

			// Enfilage
			file.push({nc, c});
		}
	}

	// Retour du personnage
	carte->set(pers->coord(), pers);
	carte->set(obj, pous);

	return resultat;
}

std::vector<bool> Solveur3::zone_sr(std::shared_ptr<moteur::Carte> carte) const {
	// Calcul
	std::vector<bool> resultat(carte->taille_x() * carte->taille_y(), false);

	for (auto pous : m_carte->liste<moteur::Poussable>()) {
		std::vector<bool> r = zone_sr(carte, pous->coord());

		for (int i = 0; i < carte->taille_x() * carte->taille_y(); ++i) {
			resultat[i] = r[i] || resultat[i];
		}
	}

	return resultat;
}

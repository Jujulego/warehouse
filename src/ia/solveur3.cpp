// Importations
#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <vector>

#include <iomanip>
#include <iostream>
#include "affichage.hpp"

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "outils.hpp"
#include "outils/console.hpp"
#include "outils/coord.hpp"
#include "outils/hongrois.hpp"
#include "outils/matrice.hpp"
#include "outils/nombre.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"
#include "hashtable.hpp"
#include "noeud.hpp"
#include "solveur3.hpp"

// Namespace
using namespace ia;
using namespace std::chrono;

// Macros
#define MAJ_AFF  0ms
#define PRIO_MAX 9

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

	for (auto p : prios) {
		dirs |= get_mask(p - pos);
	}

	return dirs;
}

// Méthodes
Chemin Solveur3::resoudre(posstream<std::ostream>& stream) {
	// Outils
	struct Etat {
		// Attributs
		std::shared_ptr<Noeud> noeud;
		unsigned dist;        // Distance parcourue
		Nombre<unsigned> heu; // Resultat de l'heuristique
		Coord pous;           // Dernier poussable traité

		// Opérateurs
		bool operator < (Etat const& e) const {
			return heu > e.heu;
		}
	};

	// Initialisation, A*
	std::priority_queue<Etat> file;
	HashTable historique;

	file.push(Etat { std::make_shared<Noeud>(), 0, heuristique(m_carte), m_obj->coord() });
	historique.insert(reduire(m_carte));

	// Stats
	steady_clock::time_point debut   = steady_clock::now();
	steady_clock::time_point der_aff = steady_clock::now();
	int noeuds_traites = 0, noeuds_a_traites = 1;
	Nombre<unsigned> min_heu = INFINI;

	// Algorithme
	while (!file.empty()) {
		// Interruption ?
		if (m_interruption) break;

		// Dépilage
		Etat etat = file.top();
		file.pop();

		// Stats
		++noeuds_traites;
		min_heu = std::min(min_heu, etat.heu);

		// Calcul de la carte
		Coord obj = m_obj->coord();
		std::shared_ptr<Noeud> noeud = etat.noeud;
		std::shared_ptr<moteur::Carte> noeud_carte = noeud->carte(m_carte, obj, m_obj->force());

		{ auto lck = console::lock();
			afficher_carte(noeud_carte, 5, 20);
			std::cout << " " << etat.heu << "      ";
		}

		for (Mouv mvt : mouvements(noeud_carte, etat.pous)) {
			// Copie de la carte
			auto carte = std::make_shared<moteur::Carte>(*noeud_carte);
			auto pous = carte->get<moteur::Poussable>(mvt.poussable->coord());
			auto pers = carte->personnage();

			// Calcul du chemin
			Chemin ch = conversion(carte, mvt.chemin, mvt.poussable->coord(), pers->coord(), pers->force());

			// Application du mouvement
			bool goal_cut = mvt.goal_move && (infos_empls(pous->coord()).zone == 0);
			Chemin chemin;

			for (auto dir : ch) {
				pers->deplacer(dir);
				chemin.ajouter(dir);

//				if (goal_cut && infos_empls(pers->coord()).zone != 0) break;
				if (goal_cut && infos_empls(pous->coord()).zone != 0) break;
			}

			if (goal_cut) {
				// Bonne association
				Coord c = pous->coord();
				Coord empl = choix_empl(carte, c);

				// Calcul du chemin
				ch = Chemin();
				if (trouver_chemin(carte, c, empl, ch, pers->force())) {
					ch = conversion(carte, ch, pous->coord(), pers->coord(), pers->force());

					for (auto dir : ch) {
						pers->deplacer(dir);
						chemin.ajouter(dir);
					}
				}
			}

			// Ajout à l'historique
			auto pair = historique.insert(reduire(carte));
			if (!pair.second) continue; // Déjà traité !

			// Ignoré si deadlock
			if (deadlock(carte, pous, pers->coord(), pers->force())) continue;

			// Ignoré si heuristique == INFINI
			Nombre<unsigned> heu = heuristique(carte);
			if (heu == INFINI) continue;

			// Fini ?
			if (carte->test_fin()) {
				auto lock = console::lock();

				// nb de noeuds traités
				stream << manip::eff_ligne;
				stream << ((double) noeuds_traites) / noeuds_a_traites * 100 << " % (" << noeuds_traites << " / " << noeuds_a_traites << ")";

				// temps passé
				stream << " en " << duration_cast<milliseconds>(steady_clock::now() - debut).count() << " ms";

				// Calcul du résultat
				Chemin res = noeud->chemin_complet();
				res.ajouter(chemin);

				return res;
			}

			{ auto lck = console::lock();
				afficher_carte(carte, 55, 20);
				std::cout << " " << mvt.heuristique << " " << heu << " " << goal_cut << "      ";
				console::getch();
			}

			// Enfilage !
			file.push(Etat {
				std::make_shared<Noeud>(chemin, noeud),
				etat.dist + chemin.longueur(),
				heu,
				pous->coord()
			});

			++noeuds_a_traites;
		}

		// Affichage
		if (steady_clock::now() - der_aff > MAJ_AFF) {
			der_aff = steady_clock::now();

			auto lock = console::lock();

			// nb de noeuds traités
			stream << manip::eff_ligne;
			stream << ((double) noeuds_traites) / noeuds_a_traites * 100 << " % (" << noeuds_traites << " / " << noeuds_a_traites << ") " << min_heu;

			min_heu = INFINI;
		}
	}

	return Chemin();
}

std::vector<Solveur3::Infos> const& Solveur3::infos_cases() const {
	// Cache
	if (c_infos.size() != 0) return c_infos;

	// Création du tableau
	c_infos.resize(m_carte->taille_x() * m_carte->taille_y(), Infos());

	// Vidage de la carte
	std::vector<std::shared_ptr<moteur::Deplacable>> deplacables;
	for (auto depl : m_carte->liste<moteur::Deplacable>()) {
		(*m_carte)[depl->coord()]->pop();
		deplacables.push_back(depl);
	}

	// Evaluation de la distance aux emplacements (pour poussables) => base heuristique
	auto pous = std::make_shared<moteur::Poussable>(m_carte.get(), 1);

	for (auto empl : m_carte->liste<moteur::Emplacement>()) {
		// Initialisation BFS
		std::map<std::pair<Coord,Coord>,bool,std::function<bool(std::pair<Coord,Coord> const&,std::pair<Coord,Coord> const&)>> marques(
			[this] (std::pair<Coord,Coord> const& p1, std::pair<Coord,Coord> const& p2) {
				return (hash(p1.first) == hash(p2.first)) ? (hash(p1.second) < hash(p2.second)) : (hash(p1.first) < hash(p2.first));
			}
		);
		std::queue<std::pair<Coord,Coord>> file;

		// Points de départs
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			Coord nc = empl->coord();
			Coord np = empl->coord() - dir;

			// Check
			if (m_carte->get<moteur::Obstacle>(np)) continue;

			// Initialisation
			c_infos[hash(nc)].distances.emplace(empl->coord(), std::pair<Coord,int>(np, 0));
			marques[{nc, np}] = true;
			file.push({nc, np});
		}

		// Algo !
		while (!file.empty()) {
			// Défilage
			Coord c = file.front().first;
			Coord p = file.front().second;
			file.pop();

			// Zone accessible
			m_carte->set(c, pous);
			std::vector<bool> zone = zone_accessible(m_carte, p);
			(*m_carte)[c]->pop();

			// Evaluation de la distance des noeuds suivants
			auto pit = c_infos[hash(c)].distances.equal_range(empl->coord());
			unsigned dist = std::numeric_limits<unsigned>::max();

			for (auto it = pit.first; it != pit.second; ++it) {
				if (zone[hash(it->second.first)]) dist = std::min(dist, it->second.second);
			}

			dist += 1;

			// Noeuds suivants !
			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				Coord nc = c - dir;
				Coord np = c - 2*dir;

				// Checks
				if (!m_carte->coord_valides(nc))        continue; // validité
				if (m_carte->get<moteur::Obstacle>(nc)) continue; // La case est un espace libre

				// La case précédante est espace libre accessible
				if (m_carte->get<moteur::Obstacle>(np)) continue;
				if (!zone[hash(np)])                    continue;

				// Marques
				if (marques[{nc, np}]) continue;
				marques[{nc, np}] = true;

				// Stockage des infos
				c_infos[hash(nc)].distances.emplace(empl->coord(), std::pair<Coord,int>(np, dist));
				c_infos[hash(nc)].ajouter(empl->coord(), dir);

				// Enfilage
				file.push({nc, np});
			}
		}
	}

	// Retour des objets
	for (auto depl : deplacables) {
		m_carte->set(depl->coord(), depl);
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
	c_infos_empls.resize(infos.size(), Empl {0, ORIGINE, {}});

	// Init pos attr
	for (size_t h = 0; h < c_infos_empls.size(); ++h) {
		c_infos_empls[h].pos = hash.unhash(h);
	}

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
					c_infos_empls[hash(pc)].prios.push_back(nc);
					c_infos_empls[hash(c) ].prios.push_back(nc);
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

Nombre<unsigned> Solveur3::distance_empl(Coord const& pous, Coord const& empl, Coord const& pers) const {
	// Récupération des distances possibles
	std::multimap<Coord,std::pair<Coord,unsigned>> distances = infos_cases(pous).distances;

	// Vidage de la carte
	std::vector<std::shared_ptr<moteur::Deplacable>> deplacables;
	for (auto depl : m_carte->liste<moteur::Deplacable>()) {
		(*m_carte)[depl->coord()]->pop();
		deplacables.push_back(depl);
	}

	// Zone accessible
	m_carte->set(pous, std::make_shared<moteur::Poussable>(m_carte.get(), 1));
	std::vector<bool> zone = zone_accessible(m_carte, pers);
	(*m_carte)[pous]->pop();

	// Récupération de la distance
	auto pit = distances.equal_range(empl);
	Nombre<unsigned> dist = INFINI;

	for (auto it = pit.first; it != pit.second; ++it) {
		if (!zone[hash(it->second.first)]) continue;
		if (it->second.second < dist) dist = it->second.second;
	}

	// Retour des objets
	for (auto depl : deplacables) {
		m_carte->set(depl->coord(), depl);
	}

	return dist;
}

std::map<Coord,std::pair<Coord,Nombre<unsigned>>> Solveur3::associations(std::shared_ptr<moteur::Carte> carte) const {
	// Cache ?
	std::vector<int> red_carte = reduire(carte);
	if (!c_carte_assos.empty() && std::equal(c_carte_assos.begin(), c_carte_assos.end(), red_carte.begin())) {
		return c_assos;
	}

	// Récupération du personnage et calcul des infos
	std::vector<Infos> infos = infos_cases();
	auto pt_pers = carte->personnage();
	Coord pers = pt_pers->coord();

	// Récupération des poussables
	std::vector<Coord> poussables;
	std::set<Coord> unmovable;

	(*carte)[pers]->pop();
	for (auto pous : carte->liste<moteur::Poussable>()) {
		poussables.push_back(pous->coord());

		bool h = carte->deplacer(pous->coord(), HAUT  , pt_pers->force() - pous->poids(), true);
		bool b = carte->deplacer(pous->coord(), BAS   , pt_pers->force() - pous->poids(), true);
		bool g = carte->deplacer(pous->coord(), GAUCHE, pt_pers->force() - pous->poids(), true);
		bool d = carte->deplacer(pous->coord(), DROITE, pt_pers->force() - pous->poids(), true);

		if ((h || b) && (g || d) && (carte->get<moteur::Emplacement>(pous->coord()) != nullptr)) {
			unmovable.insert(pous->coord());
		}
	}
	carte->set(pers, pt_pers);

	// Récupération des emplacements
	std::vector<Coord> emplacements;
	std::map<Coord,unsigned> prios;
	for (auto empl : carte->liste<moteur::Emplacement>()) {
		emplacements.push_back(empl->coord());

		// Calcul priorités
		int nb = 0;
		for (auto p : infos_empls(empl->coord()).prios) {
			if ((*carte)[p]->accessible() || (p == pers)) ++nb;
		}

		prios[empl->coord()] = nb;
	}

	// On mélange !
	std::shuffle(emplacements.begin(), emplacements.end(), std::default_random_engine());
	std::shuffle(poussables.begin(),   poussables.end(),   std::default_random_engine());

	// Vidage de la carte
	std::vector<std::shared_ptr<moteur::Deplacable>> deplacables;
	for (auto depl : carte->liste<moteur::Deplacable>()) {
		if (unmovable.find(depl->coord()) == unmovable.end()) {
			(*carte)[depl->coord()]->pop();
			deplacables.push_back(depl);
		}
	}

	// Déclaration et remplissage de la matrice
	Matrice<Nombre<unsigned>> matrice(poussables.size());
	matrice.fill(INFINI);

	auto pous = std::make_shared<moteur::Poussable>(m_carte.get(), 1);
	bool ok = true;

	for (int p = 0; p < poussables.size(); ++p) {
		if (unmovable.find(poussables[p]) == unmovable.end()) {
			std::multimap<Coord,std::pair<Coord,unsigned>> distances = infos[hash(poussables[p])].distances;

			// Zone accessible
			carte->set(poussables[p], pous);
			std::vector<bool> zone = zone_accessible(carte, pers);
			(*carte)[poussables[p]]->pop();

			// Récupération de la distance
			int nb = 0;
			for (int e = 0; e < emplacements.size(); ++e) {
				auto pit = distances.equal_range(emplacements[e]);

				for (auto it = pit.first; it != pit.second; ++it) {
					if (!zone[hash(it->second.first)]) continue;

					matrice[Coord(p, e)] = std::min(matrice[Coord(p, e)], Nombre<unsigned>(it->second.second));
					++nb;
				}

				matrice[Coord(p, e)] *= PRIO_MAX - prios[emplacements[e]];
			}

			ok &= (nb != 0);

		} else {
			for (int e = 0; e < emplacements.size(); ++e) {
				if (emplacements[e] == poussables[p]) {
					matrice[Coord(p, e)] = 0;
					break;
				}
			}
		}
	}

	// Retour des objets
	for (auto depl : deplacables) {
		carte->set(depl->coord(), depl);
	}

	// Application de l'algo
	c_carte_assos = red_carte;
	c_assos.clear();

	if (ok) {
		std::set<Coord> selection = algorithme_hongrois(matrice);

		// Déduction du résultat
		for (Coord sel : selection) {
			c_assos[poussables[sel.x()]] = {emplacements[sel.y()], matrice[sel] / (PRIO_MAX - prios[emplacements[sel.y()]])};
		}
	}

	return c_assos;
}

Nombre<unsigned> Solveur3::heuristique(std::shared_ptr<moteur::Carte> carte) const {
	return heuristique(associations(carte));
}

Nombre<unsigned> Solveur3::heuristique(std::map<Coord,std::pair<Coord,Nombre<unsigned>>> const& assos) const {
	if (assos.size() == 0) return INFINI;

	Nombre<unsigned> heu = 0;
	for (auto p : assos) {
		heu += p.second.second;
	}

	return heu;
}

Coord Solveur3::choix_empl(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	return choix_empl(associations(carte), obj);
}

Coord Solveur3::choix_empl(std::map<Coord,std::pair<Coord,Nombre<unsigned>>> const& assos, Coord const& obj) const {
	Coord empl(-1, -1);

	for (auto p : assos) {
		if (p.first == obj) {
			empl = p.second.first;
		}
	}

	return empl;
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

	// Suppression des emplacements
	for (auto empl : carte->liste<moteur::Emplacement>()) {
		zone[hash(empl->coord())] = false;
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
			if (!carte->coord_valides(c - dir)) continue; // validité
			if (!z[hash(c - dir)])                continue; // accessibilité

			// Mouvement possible ?
			if (!carte->coord_valides(nc))   continue; // validité
			if (!(*carte)[nc]->accessible()) continue; // accessibilité

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

std::list<Solveur3::Mouv> Solveur3::mouvements(std::shared_ptr<moteur::Carte> carte) const {
	// Analyse générale
	std::vector<Infos> infos = infos_cases();

	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	std::vector<bool> z_a = zone_accessible(carte, pers->coord());

	std::map<Coord,std::pair<Coord,Nombre<unsigned>>> assos = associations(carte);
	Nombre<unsigned> heu = heuristique(assos);

	// Analyse par poussable
	std::list<Mouv> mvts;

	for (auto pous : carte->liste<moteur::Poussable>()) {
		// Récupération de l'emplacement associé
		Coord c = pous->coord();
		Coord empl = choix_empl(assos, c);

		// Analyses
		std::vector<bool> z_sr = zone_sr(carte, pous->coord());

		(*carte)[pous->coord()]->pop();
		std::vector<bool> z_i = zone_interdite(carte);
		carte->set(pous->coord(), pous);

		// Goal cut
		if (empl != c && z_sr[hash(empl)]) {
			// Calcul d'un chemin
			Mouv mvt = { Chemin(), 0, pous, true };

			if (trouver_chemin(carte, c, empl, mvt.chemin, pers->force())) {
				// Evaluation de l'heuristique
				Coord nc = mvt.chemin.appliquer(c);

				Nombre<unsigned> dist  = distance_empl( c, empl, pers->coord());
				Nombre<unsigned> ndist = distance_empl(nc, empl, pers->coord());

				mvt.heuristique = heu - dist + ndist;

				mvts.push_back(mvt);
			}
		}

		// On retire le personnage
		(*carte)[pers->coord()]->pop();

		// Poussees unitaires
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Checks
			if (!carte->coord_valides(c - dir)) continue; // validité
			if (!z_a[hash(c - dir)]) continue;            // accessibilité
			if (z_i[hash(c + dir)])  continue;            // deadlock ?
			if (carte->deplacer(c, dir, pers->force() - pous->poids(), true)) continue; // possible

			// Préparation
			Mouv mvt = { Chemin(), 0, pous };
			mvt.chemin.ajouter(dir);

			// tunnel macro
			if (infos[hash(c + dir)].tunnel && !infos[hash(c + dir)].intersection) {
				Coord pos = c + dir;

				while ((infos[hash(pos)].porte & get_mask(dir)) == 0) {
					pos += dir;

					// Checks
					if (!carte->coord_valides(pos)) break;
					if (!infos[hash(pos)].tunnel)   break; // On peut sortir du tunnel !
					if (infos[hash(pos)].coin)      break; // On peut rentrer dans un angle !

					// ajout au chemin
					mvt.chemin.ajouter(dir);
				}
			}

			// Evaluation de l'heuristique
			Coord nc = mvt.chemin.appliquer(c);

			Nombre<unsigned> dist  = distance_empl( c, empl, pers->coord());
			Nombre<unsigned> ndist = distance_empl(nc, empl, pers->coord());

			mvt.heuristique = heu - dist + ndist;

			mvts.push_back(mvt);
		}

		// On remet le personnage
		carte->set(pers->coord(), pers);
	}

	// Tri
	mvts.sort([] (Mouv const& m1, Mouv const& m2) {
		return m1.heuristique < m2.heuristique;
	});

	return mvts;
}

std::list<Solveur3::Mouv> Solveur3::mouvements(std::shared_ptr<moteur::Carte> carte, Coord const& pous) const {
	std::list<Mouv> mvts = mouvements(carte);

	// Tri
	mvts.sort([pous] (Mouv const& m1, Mouv const& m2) {
		int prio1 = 0;
		int prio2 = 0;

		if (m1.goal_move) {
			prio1 = 2;
		} else if (m1.poussable->coord() == pous) {
			prio1 = 1;
		}

		if (m2.goal_move) {
			prio2 = 2;
		} else if (m2.poussable->coord() == pous) {
			prio2 = 1;
		}

		return prio1 == prio2 ? m1.heuristique < m2.heuristique : prio1 > prio2;
	});

	return mvts;
}

Chemin Solveur3::conversion(std::shared_ptr<moteur::Carte> carte, Chemin const& chemin_pous, Coord pous, Coord pers, int force) const {
	// Initialisation
	carte = std::make_shared<moteur::Carte>(*carte);
	Chemin chemin_pers;

	// Calculs
	for (auto dir : chemin_pous) {
		auto pt_pers = (*carte)[pers]->pop();
		Coord npers = pous - dir;

		// Calcul du chemin
		Chemin c;
		if (!pt_pers) break;
		if (!trouver_chemin(carte, pers, npers, c)) break;

		chemin_pers.ajouter(c);
		chemin_pers.ajouter(dir);

		// Application des mouvements
		carte->set(npers, pt_pers);
		carte->deplacer(npers, dir, force);

		pers = npers + dir;
		pous += dir;
	}

	return chemin_pers;
}

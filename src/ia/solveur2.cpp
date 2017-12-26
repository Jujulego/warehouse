// Importations
#include <algorithm>
#include <chrono>
#include <limits>
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
#include "outils/console.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"
#include "hashtable.hpp"
#include "noeud.hpp"
#include "solveur2.hpp"

#include <iostream>

// Macros
#define MAJ_AFF 250
#define INFINI  std::numeric_limits<int>::max()

// Namespace
using namespace ia;
using namespace std::chrono;

// Constructeur
Solveur2::Solveur2(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: IA(carte, obj), c_min_dist_empl(carte->taille_x() * carte->taille_y(), std::pair<Coord,int>(Coord(-1, -1), -1)) {}

// Méthodes
Chemin Solveur2::resoudre(posstream<std::ostream>& stream) {
	// Structure
	struct Etat {
		std::shared_ptr<Noeud> noeud;
		std::list<Poussee> poussees;
		unsigned ordre;
		
		mutable unsigned _dist = 0;
		
		Etat(std::shared_ptr<Noeud> const& n, std::list<Poussee> const& p, unsigned o)
            : noeud(n), poussees(p), ordre(o) {}
		
		bool operator < (Etat const& e) const {
			return ordre > e.ordre;
		}
		
		unsigned dist() const {
			if (_dist == 0) {
				_dist = noeud->chemin_complet().longueur();
			}
			
			return _dist;
		}
	};
	
	// Initialisation
	std::priority_queue<Etat> file;
	HashTable historique;
	
	historique.insert(reduire(m_carte));
	file.push(Etat(
		std::make_shared<Noeud>(),
		recup_poussees(m_carte, m_obj->coord()),
		heuristique(m_carte)
	));
	
	// Analyse statique
	std::vector<bool> zone_interdite = this->zone_interdite(m_carte, m_obj->coord());
	
	// Stats
	steady_clock::time_point debut = steady_clock::now();
	int noeuds_t = 0, noeuds_at = 1, aff = 0;
	unsigned min_heu = std::numeric_limits<unsigned>::max();
	
	// Algo
	while (!file.empty()) {
		// Interruption ?
		if (m_interruption) break;
		
		// Extraction du noeud suivant
		Etat etat = file.top();
		file.pop();
		
		// Infos ...
		auto noeud = etat.noeud;
		
		// Stats ...
		noeuds_t++;
		
		// Préparation des noeuds suivants
		for (Poussee p : etat.poussees) {
			Coord obj = m_obj->coord();
			
			// Check zone interdite
			if (zone_interdite[hash(p.pos + 2 * p.dir)]) continue;
			
			// Calcul de la nouvelle carte
			std::shared_ptr<moteur::Carte> carte = noeud->carte(m_carte, obj, m_obj->force());
			
			// Suppression du personnage
			auto pers = carte->get<moteur::Personnage>(obj);
			carte->get<moteur::Immuable>(obj)->pop();
			
			// Calcul d'un chemin
			Chemin c;
			if (!trouver_chemin(carte, obj, p.pos, c)) continue;
			
			c.ajouter(p.dir);
			
			// Application du mouvement
			carte->set(p.pos, pers);
			carte->deplacer(p.pos, p.dir, m_obj->force());
			Coord poussable = p.pos + 2*p.dir;
			
			// Ajout à l'historique
			auto pair = historique.insert(reduire(carte));
			if (!pair.second) { // Si la carte à déjà été traitée ...
				continue;
			}
			
			// Ignoré si provoque un deadlock
			if (deadlock(carte, carte->get<moteur::Poussable>(poussable), poussable - p.dir, m_obj->force())) continue;
			
			// A-t-on (enfin) trouvé ?
			if (carte->test_fin()) {
				auto lck = console::lock();
				
				// nb de noeuds traités
				stream << manip::eff_ligne;
				stream << ((double) noeuds_t) / noeuds_at * 100 << " % (" << noeuds_t << " / " << noeuds_at << ")";
				
				// temps passé
				stream << " en " << duration_cast<milliseconds>(steady_clock::now() - debut).count() << " ms";
				
				// Calcul du chemin complet
				Chemin chemin = noeud->chemin_complet();
				chemin.ajouter(c);
				
				return chemin;
			}
			
			// Ajout à la liste
			file.push(Etat(
				std::make_shared<Noeud>(c, noeud),
				recup_poussees(carte, poussable - p.dir, poussable),
				heuristique(carte)
			));

			noeuds_at++;
		}

		// Affichage
        min_heu = std::min(min_heu, etat.ordre);
		aff = (aff + 1) % MAJ_AFF;
		if (!aff) {
			auto lck = console::lock();
			
			stream << manip::eff_ligne;
			stream << ((double) noeuds_t) / noeuds_at * 100 << "% (" << noeuds_t << " / " << noeuds_at << ") " << min_heu << "      ";
			min_heu = std::numeric_limits<unsigned>::max();
		}
	}

	// Y a pas de solution
	return Chemin();
}

// Outils
unsigned Solveur2::heuristique(std::shared_ptr<moteur::Carte> const& carte) const {
	// Recherche des emplacement && boites
	std::vector<Coord> empl;

	for (auto pt : carte->liste<moteur::Emplacement>()) {
		// Oh ! un emplacement ! ;)
		empl.push_back(pt->coord());
	}

	// Calcul des distances
	// il y a tjs autant d'emplacements que de boites :
	struct Boites {
		std::vector<int> distances;
		int mini = INFINI;
		int nb = 0;

		Boites(unsigned t) : distances(t, INFINI) {};

		bool operator < (Boites const& b) const {
			if (nb == b.nb) {
				return mini > b.mini;
			}

			return nb > b.nb;
		}

		int get(unsigned i) const {
			return distances[i];
		}

		void set(unsigned i, int d) {
			// Evolution du compte de distances
			if (d != INFINI) {
				++nb;
				mini = std::min(d, mini);
			} else if (distances[i] != INFINI) {
				--nb;

				for (auto v : distances) {
					mini = std::min(v, mini);
				}
			}

			distances[i] = d;
		}

		void suppr(unsigned i) {
			set(i, INFINI);
		}

		size_t size() const {
			return distances.size();
		}
	};

	std::vector<Boites> distances(empl.size(), Boites(empl.size()));
	unsigned i = 0;

	for (auto pt : carte->liste<moteur::Poussable>()) {
		auto dists = dists_empls(carte)[hash(pt->coord())];

		// Calcul !
		for (unsigned j = 0; j < empl.size(); ++j) {
			auto it = dists.find(empl[j]);

			if (it != dists.end()) {
				distances[j].set(i, it->second);
			}
		}

		// incrementation !
		++i;
	}

	// Evaluation
	std::sort(distances.begin(), distances.end());
	unsigned r = 0;

	while (!distances.empty()) {
		auto dist = distances.back();
		distances.pop_back();

		// Test !
		if (dist.nb == 0) return std::numeric_limits<unsigned>::max(); // Niveau impossible !

		// Recherche du minimum
		unsigned mini = dist.size();

		for (unsigned j = 0; j < dist.size(); ++j) {
			if (mini == dist.size() || dist.get(mini) > dist.get(j)) {
				mini = j;
			}
		}

		if (mini != dist.size()) {
			r += dist.get(mini);

			// suppression des autres tableaux
			for (unsigned j = 0; j < distances.size(); ++j) {
				distances[j].suppr(mini);
			}

			std::sort(distances.begin(), distances.end());
		} else {
			return std::numeric_limits<unsigned>::max();
		}
	}

	return r;
}

std::list<Solveur2::Poussee> Solveur2::recup_poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj, Coord const& /*prec*/) const {
	// Evaluation des positions accessibles
	std::vector<bool> zone = zone_accessible(carte, obj);
	
	// Suppression du personnage
	carte = std::make_shared<moteur::Carte>(*carte);
	
	auto pers = carte->get<moteur::Deplacable>(obj);
	(*carte)[pers->coord()]->pop();
	
	// Parcours des boites:
	std::list<Poussee> poussees;
	
	for (auto pt : carte->liste<moteur::Poussable>()) {
		// Les 4 directions !
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// La case du joueur
			Coord pos = pt->coord() - dir;
			
			// Checks
			if (!carte->coord_valides(pos)) continue; // valide ?
			if (!zone[hash(pos)])           continue; // accessible ?
			if (carte->deplacer(pt->coord(), dir, pers->force() - pt->poids(), true)) continue;
			
			// Ajout trié
//			if (pos + dir == prec) {
//				poussees.push_front({pos, dir});
//			} else {
				poussees.push_back({pos, dir});
//			}
			poussees.sort([this,&carte] (Poussee const& p1, Poussee const& p2) -> bool {
				return min_dist_empl(carte, p1.pos + p1.dir).second < min_dist_empl(carte, p2.pos + p2.dir).second;
			});
		}
	}
	
	return poussees;
}

bool Solveur2::est_tunnel(std::shared_ptr<moteur::Carte> const& carte, Coord const& pos, Coord const& dir) const {
	bool tunnel = carte->get<moteur::Obstacle>(pos) != nullptr;
	
	for (auto d : {HAUT, BAS, GAUCHE, DROITE}) {
		if (dir == d || dir == -d) continue;
		tunnel &= carte->get<moteur::Obstacle>(pos + d) != nullptr;
	}
	
	return tunnel;
}

std::pair<Coord,int> Solveur2::min_dist_empl(std::shared_ptr<moteur::Carte> const& carte, Coord const& pos) const {
	// Cache
	if (c_min_dist_empl[hash(pos)].second != -1) return c_min_dist_empl[hash(pos)];
	
	// Evaluation du minimum
	int mini = std::numeric_limits<int>::max();
	Coord empl(-1, -1);

	auto de = dists_empls(carte);
	for (auto p : de[hash(pos)]) {
		if (mini > p.second) {
			mini = p.second;
			empl = p.first;
		}
	}
	
	// Enregistrement
	c_min_dist_empl[hash(pos)] = std::pair<Coord,int>(empl, mini);
	
	return std::pair<Coord,int>(empl, mini);
}

std::vector<std::map<Coord,int>> Solveur2::dists_empls(std::shared_ptr<moteur::Carte> const& carte) const {
	// Cache
	if (c_dists_empls.size() != 0) return c_dists_empls;
	
	// Déclarations
	std::vector<std::map<Coord,int>> dists(
		carte->taille_x() * carte->taille_y(),
		std::map<Coord,int>()
	);
	
	// Parcours des emplacements
	for (auto empl : carte->liste<moteur::Emplacement>()) {
		// Init BFS
		std::queue<Coord> file;
		
		dists[hash(empl->coord())][empl->coord()] = 0;
		file.push(empl->coord());
		
		// Algo !
		while (!file.empty()) {
			// Défilage
			Coord c = file.front();
			file.pop();
			
			// Evalutation des suivants !
			int nd = dists[hash(c)][empl->coord()] + 1;
			
			for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
				// Les 2 précédantes cases doivent être des espaces (pas mur), pour pouvoir réaliser une poussée
				if (carte->get<moteur::Obstacle>(c - dir) || carte->get<moteur::Obstacle>(c - 2*dir)) {
					continue; // suivant !
				}
				
				// Marquage
				auto p = dists[hash(c - dir)].emplace(empl->coord(), nd);
				if (!p.second) continue; // Déjà traité !
				
				// Emfilage
				file.push(c - dir);
			}
		}
	}
	
	// Sauvegarde du résultat
	c_dists_empls = dists;
	
	return dists;
}

std::vector<bool> Solveur2::zone_interdite(std::shared_ptr<moteur::Carte> const& carte, Coord const& dep) const {
	// Cache
	if (c_zone_interdite.size() != 0) return c_zone_interdite;
	
	// Déclarations
	std::vector<bool> marques(carte->taille_x() * carte->taille_y(), false);
	std::vector<bool> zone(carte->taille_x() * carte->taille_y(), false);
	
	// DFS
	std::stack<Coord> pile;
	
	marques[hash(dep)] = true;
	pile.push(dep);
	
	while (!pile.empty()) {
		// Dépilage
		Coord c = pile.top();
		pile.pop();
		
		// Interdite ?
		zone[hash(c)] = dists_empls(carte)[hash(c)].size() == 0;
		
		// Suivants !
		for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
			// Checks
			if (!carte->coord_valides(c + dir)) continue; // validité
			if (marques[hash(c + dir)])         continue; // marquage
			if (carte->get<moteur::Obstacle>(c + dir) != nullptr) continue; // pas un mur
			
			// Empilage
			marques[hash(c + dir)] = true;
			pile.push(c + dir);
		}
	}
	
	// Sauvegarde du résultat
	c_zone_interdite = zone;
	
	return zone;
}

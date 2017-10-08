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
#include "moteur/poussable.hpp"
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
#define MAJ_AFF 500
#define INFINI  std::numeric_limits<int>::max()

// Namespace
using namespace ia;
using namespace std::chrono;

// Constructeur
Solveur2::Solveur2(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: IA(carte, obj) {}

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
			if (ordre == e.ordre) {
				return dist() > e.dist();
			}

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
		heuristique(m_carte, m_obj->force())
	));

	// Stats
	steady_clock::time_point debut = steady_clock::now();
	int noeuds_t = 0, noeuds_at = 1, aff = 0;

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

			// Calcul de la nouvelle carte
			std::shared_ptr<moteur::Carte> carte = noeud->carte(m_carte, obj, m_obj->force());

			// Suppression du personnage
			auto pers = carte->get<moteur::Deplacable>(obj);
			carte->get<moteur::Immuable>(obj)->pop();

			// Calcul d'un chemin
			Chemin c;
			if (!trouver_chemin(carte, obj, p.pos, c)) continue;

			c.ajouter(p.dir);

			// Application du mouvement
			carte->set(p.pos, pers);
			carte->deplacer(p.pos, p.dir, m_obj->force());

			// Ajout à l'historique
			auto pair = historique.insert(reduire(carte));
			if (!pair.second) { // Si la carte à déjà été traitée ...
				continue;
			}

			// Ignoré si provoque un deadlock
			if (deadlock(carte, carte->get<moteur::Poussable>(p.pos + 2 * p.dir), p.pos + p.dir, m_obj->force())) continue;
			
			// A-t-on (enfin) trouvé ?
			if (carte->test_fin()) {
				auto lck = console::lock();
				
				// nb de noeuds traités
				stream << manip::eff_ligne;
				stream << noeuds_t << " / " << noeuds_at;

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
				recup_poussees(carte, p.pos + p.dir),
				heuristique(carte, m_obj->force())
			));

			noeuds_at++;
		}

		// Affichage
		aff = (aff + 1) % MAJ_AFF;
		if (!aff) {
			auto lck = console::lock();
			
			stream << manip::eff_ligne;
			stream << noeuds_t << " / " << noeuds_at;
		}
	}

	// Y a pas de solution
	return Chemin();
}

// Outils
unsigned Solveur2::heuristique(std::shared_ptr<moteur::Carte> const& carte, int force) const {
	// Evaluation des positions accessibles
	std::hash<Coord> hash(carte->taille_y());

	// Recherche des emplacement && boites
	std::vector<Coord> empl;

	for (auto pt : carte->liste<moteur::Emplacement>()) {
		// Oh ! un emplacement !
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

	auto print = [&] () -> void {
		for (unsigned i = 0; i < empl.size(); i++) {
			for (unsigned j = 0; j < empl.size(); j++) {
				std::cout << manip::coord(40 + 3*i, 21 + j);
				std::cout << "   ";
				std::cout << manip::coord(40 + 3*i, 21 + j);

				if (i < distances.size() && distances[i].get(j) != INFINI) {
					std::cout << distances[i].get(j);
				}
			}
		}
	};

	unsigned k = 0;
	for (auto pt : carte->liste<moteur::Poussable>()) {
//		std::cout << manip::coord(37, 21 + (k++)) << (char) (pt->coord().x() + 'A') << pt->coord().y();

		// Calcul !
		for (unsigned j = 0; j < empl.size(); ++j) {
//			Chemin chemin;
//			if (!trouver_chemin(carte, pt->coord(), empl[j], chemin, force)) continue;

//			distances[j].set(i, chemin.longueur());
			distances[j].set(i, std::abs(pt->coord().x() - empl[j].x()) + std::abs(pt->coord().y() - empl[j].y()));
		}

		// incrementation !
		++i;
	}

	// Evaluation
	std::sort(distances.begin(), distances.end());
	unsigned r = 0;

	while (!distances.empty()) {
/*		print();
		std::cin.get();*/

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

std::list<Solveur2::Poussee> Solveur2::recup_poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
	// Evaluation des positions accessibles
	std::vector<bool> zone = zone_accessible(carte, obj);
	std::hash<Coord> hash(carte->taille_y());

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

			// Ajout
			poussees.push_back({pos, dir});
		}
	}

	return poussees;
}

std::vector<bool> Solveur2::zone_accessible(std::shared_ptr<moteur::Carte> const& carte, Coord const& obj) const {
	// Déclarations
	std::vector<bool> zone(carte->taille_x() * carte->taille_y(), false);
	std::hash<Coord> hash(carte->taille_y());

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

// Importations
#include <algorithm>
#include <cmath>
#include <future>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"
#include "outils/coord.hpp"

#include "ia.hpp"
#include "noeud.hpp"

// Namespace
using namespace ia;
using namespace std::placeholders;

// Alias
template<class T, class U>
inline std::shared_ptr<T> cast(std::shared_ptr<U> const& pt) {
	return std::dynamic_pointer_cast<T>(pt);
}

// Constructeur
IA::IA(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: hash(carte->taille_y()), m_carte(carte), m_obj(obj), m_interruption(false) {};

IA::IA(IA&& ia) : hash(ia.hash), m_carte(ia.m_carte), m_obj(ia.m_obj), m_interruption(ia.m_interruption.load()) {};

// Opérateur
IA& IA::operator = (IA&& ia) {
	// Copie des données
	hash    = ia.hash;
	m_carte = ia.m_carte;
	m_obj   = ia.m_obj;
	m_interruption = ia.m_interruption.load();

	return *this;
}

// Méthodes
std::future<Chemin> IA::async_resoudre(posstream<std::ostream>& stream) {
	m_interruption = false;
	return std::async(std::launch::async, &IA::resoudre, this, std::ref(stream));
}

void IA::interrompre() {
	m_interruption = true;
}

// Outils
std::vector<bool> IA::zone_accessible(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const {
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

bool IA::deadlock(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Poussable> const& pt, Coord const& pers, int force) const {
	if (carte->get<moteur::Emplacement>(pt->coord())) return false;

	// On retire le personnage
	std::shared_ptr<moteur::Deplacable> pt_pers;
	if (carte->coord_valides(pers)) pt_pers = (*carte)[pers]->pop();

	// Marques
	std::set<std::shared_ptr<moteur::Poussable>> marques;

	// Fonction récursive :
	std::function<bool(std::shared_ptr<moteur::Poussable> const&,Coord)> exec(
	[&] (std::shared_ptr<moteur::Poussable> const& p, Coord prov) -> bool {
		// Si la boite ne peux plus être déplacée => deadlock
		bool err = true;
		for (Coord dir : {HAUT, BAS, GAUCHE, DROITE}) {
			if (dir == -prov) continue;

			if (!carte->coord_valides(p->coord() - dir) || carte->get<moteur::Obstacle>(p->coord() - dir)) {
				continue;
			}

			auto pous = carte->get<moteur::Poussable>(p->coord() - dir);
			if (pous) {
				auto pair = marques.insert(pous);
				if (dir != prov && !pair.second) return true;

				if (pair.second && exec(pous, dir)) {
					continue;
				}
			}

			err &= carte->deplacer(p->coord(), dir, force - p->poids(), true);
			if (!err) break;
		}

		return err;
	});

	// Execution
	marques.insert(pt);
	bool res = exec(pt, ORIGINE);

	// Et on remet le personnage
	if (carte->coord_valides(pers)) carte->set(pers, pt_pers);

	return res;
}

bool IA::trouver_chemin(std::shared_ptr<moteur::Carte> carte, Coord const& dep, Coord const& arr, Chemin& res) const {
	// Cas de base
	if (!carte->coord_valides(dep)) return false;
	if (!carte->coord_valides(arr)) return false;
	if (dep == arr) return true;

	// Outils
	using Etat = std::pair<int,std::shared_ptr<Noeud>>;
	struct Ordre {
		bool operator () (Etat const& e1, Etat const& e2) const {
			return e1.first > e2.first;
		}
	};

	auto distance = [&arr] (Coord const& c) -> int {
		return std::abs(arr.x() - c.x()) + std::abs(arr.y() - c.y());
	};

	// Recherche d'un chemin SANS pousser de boite
	std::unordered_set<Coord> marques(0, hash);
	std::priority_queue<Etat,std::vector<Etat>,Ordre> file;

	// Algo
	file.push(Etat { distance(dep), std::make_shared<Noeud>() });
	marques.insert(dep);

	while (!file.empty()) {
		// Depilage
		std::shared_ptr<Noeud> n = file.top().second;
		file.pop();

		// Calcul coordonnées actuelle (+carte ;)
		std::shared_ptr<moteur::Carte> carte_tmp;
		Coord c = dep;

		c = n->chemin_complet().appliquer(dep);

		// Traitement
		for (auto m : {HAUT, BAS, DROITE, GAUCHE}) {
			Coord nc = c + m;

			// Checks
			if (!carte->coord_valides(nc))   continue;
			if (!(*carte)[nc]->accessible()) continue;

			// Test de fin
			if (nc == arr) {
				res = n->chemin_complet();
				res.ajouter(m);

				return true;
			}

			// Check marque
			auto p = marques.insert(nc);
			if (!p.second) continue;

			// Ajout au traitement
			file.push(Etat { distance(nc), std::make_shared<Noeud>(m, n) });
		}
	}

	return false;
}

bool IA::trouver_chemin(std::shared_ptr<moteur::Carte> carte, Coord const& dep, Coord const& arr, Chemin& res, int force) const {
	// Cas de base
	if (!carte->coord_valides(dep)) return false;
	if (!carte->coord_valides(arr)) return false;
	if (dep == arr) return true;

	// Personnage ?
	if (force == 0) return trouver_chemin(carte, dep, arr, res);

	// Outils
	using Etat = std::tuple<int,std::shared_ptr<Noeud>,Coord>;
	struct Ordre {
		bool operator () (Etat const& e1, Etat const& e2) const {
			return std::get<0>(e1) > std::get<0>(e2);
		}
	};

	auto distance = [&arr] (Coord const& c) -> int {
		return std::abs(arr.x() - c.x()) + std::abs(arr.y() - c.y());
	};

	// Extraction du poussable
	std::shared_ptr<moteur::Poussable> pous = carte->get<moteur::Poussable>(dep);
	if (pous == nullptr) return trouver_chemin(carte, dep, arr, res); // pas de poussable
	(*carte)[dep]->pop();

	// Extraction du personnage
	std::shared_ptr<moteur::Personnage> pers = carte->personnage();
	(*carte)[pers->coord()]->pop();

	// Recherche d'un chemin SANS pousser de boite
	std::map<std::pair<Coord,Coord>,bool,std::function<bool(std::pair<Coord,Coord> const&,std::pair<Coord,Coord> const&)>> marques(
		[this] (std::pair<Coord,Coord> const& p1, std::pair<Coord,Coord> const& p2) {
			return (hash(p1.first) == hash(p2.first)) ? (hash(p1.second) < hash(p2.second)) : (hash(p1.first) < hash(p2.first));
		}
	);
	std::priority_queue<Etat,std::vector<Etat>,Ordre> file;

	// Algo
	file.push(Etat {distance(dep), std::make_shared<Noeud>(), pers->coord()});
	marques[{dep, pers->coord()}] = true;

	while (!file.empty()) {
		// Défilage
		std::shared_ptr<Noeud> n = std::get<1>(file.top());
		Coord p = std::get<2>(file.top());
		file.pop();

		// Calcul coordonnées actuelle (+carte ;)
		std::shared_ptr<moteur::Carte> carte_tmp;
		Coord c = dep;

		c = n->chemin_complet().appliquer(dep);

		// Calcul zone accessible
		carte->set(c, std::make_shared<moteur::Poussable>(carte.get(), pous->poids()));
		std::vector<bool> z = zone_accessible(carte, p);
		(*carte)[c]->pop();

		// Traitement
		for (auto m : {HAUT, BAS, DROITE, GAUCHE}) {
			Coord nc = c + m;

			// Checks
			if (!carte->coord_valides(c - m)) continue; // validité
			if (!z[hash(c - m)])              continue; // accessibilité

			if (!carte->coord_valides(nc))   continue; // validité
			if (!(*carte)[nc]->accessible()) continue; // accessibilité

			// Test de fin
			if (nc == arr) {
				res = n->chemin_complet();
				res.ajouter(m);

				// Retour du personnage et du poussable
				carte->set(pers->coord(), pers);
				carte->set(dep, pous);

				return true;
			}

			// Check marque
			if (marques[{nc, c}]) continue; // Déjà traité
			marques[{nc, c}] = true;

			// Ajout au traitement
			file.push(Etat { distance(nc), std::make_shared<Noeud>(m, n), c });
		}
	}

	// Retour du personnage et du poussable
	carte->set(pers->coord(), pers);
	carte->set(dep, pous);

	return false;
}

std::vector<int> IA::reduire(std::shared_ptr<moteur::Carte> const& carte, bool pos_pers) const {
	// Déclarations
	int tc = carte->taille_x() * carte->taille_y();
	std::vector<int> reduction(1, 0);

	// Réduction !
	for (auto pt : carte->liste<moteur::Poussable>()) {
		reduction.push_back(pt->poids() * tc + hash(pt->coord()));
	}

	// Infos personnage
	Coord p = carte->personnage()->coord();
	if (pos_pers) {
		reduction.push_back(hash(p));

	} else {
		// Stockage de la "zone" => coord 1ere case zone accessible
		std::vector<bool> zone = zone_accessible(carte, p);
		auto it = std::find(zone.begin(), zone.end(), true);
		reduction.push_back(zone.begin() - it);
	}

	// Tri !
	std::sort(reduction.begin()+1, reduction.end());

	return reduction;
}

std::list<Coord> IA::mouvements(std::shared_ptr<moteur::Deplacable> const& obj) const {
	std::list<Coord> mvts;

	for (Coord m : {HAUT, BAS, GAUCHE, DROITE}) {
		if (!obj->deplacer(m, true)) mvts.push_back(m);
	}

	return mvts;
}

bool IA::comparer(std::shared_ptr<moteur::Carte> const& c1, std::shared_ptr<moteur::Carte> const& c2) const {
	// Je suppose que les immuables sont les memes dans les 2 cartes
	bool equiv = true;

	for (auto pt : c1->liste<moteur::Deplacable>()) {
		// Ils faut que les 2 soient du même type
		     if (cast<moteur::Personnage>(pt) != nullptr) equiv &= (c2->get<moteur::Personnage>(pt->coord()) != nullptr);
		else if (cast<moteur::Poussable>( pt) != nullptr) equiv &= (c2->get<moteur::Poussable>( pt->coord()) != nullptr);

		// Break ?
		if (!equiv) break;
	}

	return equiv;
}

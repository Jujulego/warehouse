// Importations
#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <memory>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"
#include "outils/coord.hpp"

#include "ia.hpp"
#include "noeud.hpp"

// Namespace
using namespace ia;

// Alias
template<class T, class U>
inline std::shared_ptr<T> cast(std::shared_ptr<U> const& pt) {
	return std::dynamic_pointer_cast<T>(pt);
}

// Constructeur
IA::IA(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: m_carte(carte), m_obj(obj) {};

// Outils
std::vector<int> IA::reduire(std::shared_ptr<moteur::Carte> const& carte) const {
	// Déclarations
	int tc = carte->taille_x() * carte->taille_y();
	std::hash<Coord> hash(carte->taille_y());
	std::vector<int> reduction(1, 0);
	
	// Personnage
	for (auto pt : carte->liste<moteur::Deplacable>()) {
		Coord c = pt->coord();
		
		// Réduction !
		     if (cast<moteur::Personnage>(pt) != nullptr) reduction[0] = hash(c);
		else if (cast<moteur::Poussable>( pt) != nullptr) reduction.push_back(cast<moteur::Poussable>(pt)->poids() * tc + hash(c));
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

bool IA::trouver_chemin(std::shared_ptr<moteur::Carte> const& carte, Coord const& dep, Coord const& arr, Chemin& res) const {
	// Cas de base
	if (!carte->coord_valides(dep)) return false;
	if (!carte->coord_valides(arr)) return false;
	if (dep == arr) return true;
	
	// Outils
	using Etat = std::pair<int,std::shared_ptr<Noeud>>;
	struct Ordre {
		bool operator () (Etat const& e1, Etat const& e2) const {
			return e1.first < e2.first;
		}
	};
	
	auto distance = [&arr] (Coord const& c) -> int {
		return std::abs(arr.x() - c.x()) + std::abs(arr.y() - c.y());
	};
	
	// Recherche d'un chemin SANS pousser de boite
	std::unordered_set<Coord> marques(0, std::hash<Coord>(carte->taille_y()));
	std::priority_queue<Etat,std::vector<Etat>,Ordre> file;
	
	// Algo
	file.push(Etat { distance(dep), std::make_shared<Noeud>() });
	marques.insert(dep);
	
	while (!file.empty()) {
		// Depilage
		std::shared_ptr<Noeud> n = file.top().second;
		file.pop();
		
		Coord c = n->chemin_complet().appliquer(dep);
		
		// Test de fin
		if (c == arr) {
			res = n->chemin_complet();
			return true;
		}
		
		// Traitement
		for (auto m : {HAUT, BAS, DROITE, GAUCHE}) {
			Coord nc = c + m;
			
			// Checks
			if (!carte->coord_valides(nc)) continue;
			if (!carte->get<moteur::Immuable>(nc)->accessible()) continue;
			
			// Check marque
			auto p = marques.insert(nc);
			if (!p.second) continue;
			
			// Ajout au traitement
			file.push(Etat { distance(nc), std::make_shared<Noeud>(m, n) });
		}
	}
	
	return false;
}

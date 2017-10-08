// Importations
#include <algorithm>
#include <cmath>
#include <future>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <queue>
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
	: m_carte(carte), m_obj(obj), m_interruption(false) {};

IA::IA(IA&& ia) : m_carte(ia.m_carte), m_obj(ia.m_obj), m_interruption(ia.m_interruption.load()) {};

// Opérateur
IA& IA::operator = (IA&& ia) {
	// Copie des données
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

bool IA::trouver_chemin(std::shared_ptr<moteur::Carte> carte, Coord const& dep, Coord const& arr, Chemin& res, int force) const {
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
	std::unordered_set<Coord> marques(0, std::hash<Coord>(carte->taille_y()));
	std::priority_queue<Etat,std::vector<Etat>,Ordre> file;
	int poids = 0;
	
	if (force) {
		poids = carte->get<moteur::Poussable>(dep)->poids();
		
		// suppression du Personnage
		carte = std::make_shared<moteur::Carte>(*carte);
		
		auto pers = carte->personnage();
		if (pers) (*carte)[pers->coord()]->pop();
	}
	
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
		
		if (force) {
			carte_tmp = n->carte(carte, c, force);
		} else {
			c = n->chemin_complet().appliquer(dep);
		}
		
		// Traitement
		for (auto m : {HAUT, BAS, DROITE, GAUCHE}) {
			Coord nc = c + m;
			
			// Checks
			if (!carte->coord_valides(nc)) continue;
			
			if (force) { // Il faut pousser !
				if (!carte_tmp->coord_valides(c - m)) continue;
				if (!carte_tmp->get<moteur::Immuable>(c - m)->accessible()) continue;
				if ( carte_tmp->deplacer(c, m, force - poids, true)) continue;
			} else {     // pour le personnage
				if (!carte->get<moteur::Immuable>(nc)->accessible()) continue;
			}
			
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

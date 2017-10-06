// Importations
#include <list>
#include <memory>
#include <stack>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/poussable.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"
#include "ia.hpp"
#include "solveur2.hpp"

// Namespace
using namespace ia;

// Constructeur
Solveur2::Solveur2(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: IA(carte, obj) {}

// Méthodes
Chemin Solveur2::resoudre() {
	// Plus tard !
}

// Methodes
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

std::list<Solveur2::Poussee> Solveur2::recup_poussees(std::shared_ptr<moteur::Carte> const& carte, Coord const& obj) const {
	// Evaluation des positions accessibles
	std::vector<bool> zone = zone_accessible(carte, obj);
	std::hash<Coord> hash(carte->taille_y());
	auto pers = carte->personnage();
	
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

// Importations
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"

#include "chemin.hpp"
#include "noeud.hpp"
#include "solveur.hpp"

// Namespace
using namespace ia;

// Outils
struct Hash {
	// Opérateur d'appel
	size_t operator () (std::vector<int> const& r) const {
		size_t resultat = 0;
		
		for (unsigned i = r.size(); i > 0; --i) {
			resultat += i * r[i-1];
		}
		
		return resultat;
	}
};

struct Egal {
	// Opérateur d'appel
	bool operator () (std::vector<int> const& r1, std::vector<int> const& r2) const {
		if (r1.size() != r2.size()) return false;
		
		for (unsigned i = 0; i < r1.size(); ++i) {
			if (r1[i] != r2[i]) return false;
		}
		
		return true;
	}
};

// Constructeur
Solveur::Solveur(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: IA(carte, obj) {}

// Méthodes
Chemin Solveur::resoudre() {
	// Initialisation
	std::unordered_set<std::vector<int>,Hash,Egal> historique;
	std::queue<std::shared_ptr<Noeud>> file;
	file.push(std::make_shared<Noeud>());
	
	// Algo
	while (!file.empty()) {
		// Extraction du noeud suivant
		auto noeud = file.front();
		file.pop();
		
		// Calcul de la nouvelle carte
		Coord obj = m_obj->coord();
		std::shared_ptr<moteur::Carte> carte = noeud->carte(m_carte, obj, m_obj->force());
		
		// A-t-on (enfin) trouvé ?
		if (carte->test_fin()) return noeud->chemin_complet();
		
		// Ajout à l'historique
		auto p = historique.insert(reduire(carte));
		if (!p.second) { // Si la carte à déjà été traitée ...
			continue;
		}
		
		// Si une boite touche 2 murs en angle sans etre sur un emplacement => deadlock !
		bool deadlock = false;
		for (auto pt : carte->liste<moteur::Poussable>()) {
			if (carte->get<moteur::Emplacement>(pt->coord())) continue;
			
			bool mur = false;
			for (Coord dir : {HAUT, BAS}) {
				if (carte->get<moteur::Obstacle>(pt->coord() + dir)) mur = true;
			}
			
			if (mur) {
				mur = false;
				
				for (Coord dir : {GAUCHE, DROITE}) {
					if (carte->get<moteur::Obstacle>(pt->coord() + dir)) mur = true;
				}
				
				if (mur) {
					deadlock = true;
					break;
				}
			}
		}
		
		if (deadlock) continue;
		
		// Préparation des noeuds suivants
		for (Coord m : mouvements(carte->get<moteur::Deplacable>(obj))) {
			file.push(std::make_shared<Noeud>(m, noeud));
		}
	}
	
	// Y a pas de solution
	return Chemin();
}

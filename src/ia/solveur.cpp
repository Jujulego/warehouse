// Importations
#include <memory>
#include <queue>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"

#include "chemin.hpp"
#include "noeud.hpp"
#include "solveur.hpp"

// Namespace
using namespace ia;

// Constructeur
Solveur::Solveur(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: IA(carte, obj) {}

// Méthodes
Chemin Solveur::resoudre() {
	// Initialisation
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
		if (carte->test_fin()) return noeud->chemin();
		
		// Préparation des noeuds suivants
		for (Coord m : mouvements(carte->get<moteur::Deplacable>(obj))) {
			file.push(std::make_shared<Noeud>(m, noeud));
		}
	}
	
	// Y a pas de solution
	return Chemin();
}

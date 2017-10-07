// Importations
#include <chrono>
#include <memory>
#include <ostream>
#include <queue>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"
#include "hashtable.hpp"
#include "noeud.hpp"
#include "solveur.hpp"

// Macros
#define MAJ_AFF 500

// Namespace
using namespace ia;
using namespace std::chrono;

// Constructeur
Solveur::Solveur(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: IA(carte, obj) {}

// Méthodes
Chemin Solveur::resoudre(posstream<std::ostream>& stream) {
	// Initialisation
	std::queue<std::shared_ptr<Noeud>> file;
	file.push(std::make_shared<Noeud>());
	HashTable historique;
	
	// Stats
	steady_clock::time_point debut = steady_clock::now();
	int noeuds_t = 0, noeuds_at = 1, aff = 0;
	
	// Algo
	while (!file.empty()) {
		// Interruption ?
		if (m_interruption) break;
		
		// Extraction du noeud suivant
		auto noeud = file.front();
		file.pop();
		
		noeuds_t++;
		
		// Calcul de la nouvelle carte
		Coord obj = m_obj->coord();
		std::shared_ptr<moteur::Carte> carte = noeud->carte(m_carte, obj, m_obj->force());
		
		// A-t-on (enfin) trouvé ?
		if (carte->test_fin()) {
			// nb de noeuds traités
			stream << manip::eff_ligne;
			stream << noeuds_t << " / " << noeuds_at;
			
			// temps passé
			stream << " en " << duration_cast<milliseconds>(steady_clock::now() - debut).count() << " ms";
			
			return noeud->chemin_complet();
		}
		
		// Ajout à l'historique
		auto p = historique.insert(reduire(carte));
		if (!p.second) { // Si la carte à déjà été traitée ...
			continue;
		}
		
		// Arret en cas de deadlock
		if (deadlock(carte)) continue;
		
		// Préparation des noeuds suivants
		for (Coord m : mouvements(carte->get<moteur::Deplacable>(obj))) {
			file.push(std::make_shared<Noeud>(m, noeud));
			noeuds_at++;
		}
		
		// Affichage
		aff = (aff + 1) % MAJ_AFF;
		if (!aff) {
			stream << manip::eff_ligne;
			stream << noeuds_t << " / " << noeuds_at;
		}
	}
	
	// Y a pas de solution
	return Chemin();
}

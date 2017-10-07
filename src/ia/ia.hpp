#pragma once

// Importations
#include <list>
#include <memory>
#include <ostream>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"

// Namespace
namespace ia {

// Classe
class IA {
	public:
		// Constructeur
		IA(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj);
		
		// Destructeur
		virtual ~IA() = default;
		
		// Méthodes
		virtual Chemin resoudre(posstream<std::ostream>& stream) = 0;
		
		// Outils
		bool deadlock(std::shared_ptr<moteur::Carte> const& carte) const;
		bool trouver_chemin(      // Vrai = trouvé !
			std::shared_ptr<moteur::Carte> carte,
			Coord const& dep, Coord const& arr,
			Chemin& res,
			int force = 0) const; // force = 0 => pour le personnage, force != 0 => pour une boite qu'il faut pousser
	
	protected:
		// Attributs
		std::shared_ptr<moteur::Carte> m_carte;
		std::shared_ptr<moteur::Deplacable> m_obj;
		
		// Outils
		std::vector<int> reduire(std::shared_ptr<moteur::Carte> const& c) const;
		std::list<Coord> mouvements(std::shared_ptr<moteur::Deplacable> const& obj) const;
		bool comparer(std::shared_ptr<moteur::Carte> const& c1, std::shared_ptr<moteur::Carte> const& c2) const;
};

} // ia

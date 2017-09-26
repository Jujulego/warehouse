#pragma once

// Importations
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"

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
		virtual Chemin resoudre() = 0;
	
	protected:
		// Outils
		bool comparer(std::shared_ptr<moteur::Carte> const& c1, std::shared_ptr<moteur::Carte> const& c2) const;
	
	private:
		// Attributs
		std::shared_ptr<moteur::Carte> m_carte;
		std::shared_ptr<moteur::Deplacable> m_obj;
};

} // ia

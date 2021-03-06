#pragma once

// Importations
#include <memory>

#include "outils/coord.hpp"

#include "objet.hpp"

// Namespace
namespace moteur {

class Carte;

// Classe
class Deplacable : public Objet {
	public:
		// Construteur
		Deplacable(Carte* carte, int force);
		
		// Méthodes
		virtual bool deplacer(Coord const& vecteur, bool fake = false); //! Renvoie true en cas d'erreur (le mouvement n'est alors pas réalisé)
		virtual bool deplacer(Coord const& vecteur, int& push, bool fake = false);
		virtual std::shared_ptr<Deplacable> copie(Carte* carte) const;
		
		// Accesseurs
		Coord& coord();
		int force() const;
	
	private:
		// Attributs
		Carte* m_carte;
		int m_force;
};

} // moteur

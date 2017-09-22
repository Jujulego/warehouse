#pragma once

// Importations
#include "outils/coord.hpp"

#include "objet.hpp"

// Namespace
namespace moteur {

class Carte;

// Classe
class Deplacable : public Objet {
	public:
		// Construteur
		Deplacable(Carte* carte);
		
		// Méthodes
		virtual bool deplacer(Coord const& vecteur); //! Renvoie true en cas d'erreur (le mouvement n'est alors pas réalisé)
		
		// Accesseurs
		Coord& coord();
	
	private:
		// Attributs
		Carte* m_carte;
};

} // moteur
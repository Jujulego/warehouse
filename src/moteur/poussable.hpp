#pragma once

// Importations
#include "deplacable.hpp"

// Namespace
namespace moteur {

// Classe
class Poussable : public Deplacable {
	public:
		// Constructeur
		Poussable(Carte* carte, int poids);
		
		// Accesseur
		int poids() const;
	
	private:
		// Attributs
		int m_poids;
};

} // moteur

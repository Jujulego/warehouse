#pragma once

// Importations
#include <memory>

#include "deplacable.hpp"

// Namespace
namespace moteur {

// Classe
class Poussable : public Deplacable {
	public:
		// Constructeur
		Poussable(Carte* carte, int poids);
		
		// Méthodes
		virtual std::shared_ptr<Deplacable> copie(Carte* carte) const override;
		
		// Accesseur
		int poids() const;
	
	private:
		// Attributs
		int m_poids;
};

} // moteur

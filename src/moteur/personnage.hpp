#pragma once

// Importations
#include <memory>

#include "deplacable.hpp"

// Namespace
namespace moteur {

// Classe
class Personnage : public Deplacable {
	public:
		// Constructeur
		Personnage(Carte* carte, int force);
		
		// Méthodes
		virtual std::shared_ptr<Deplacable> copie(Carte* carte) const override;
};

} // moteur

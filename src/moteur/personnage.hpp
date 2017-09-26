#pragma once

// Importations
#include "deplacable.hpp"

// Namespace
namespace moteur {

// Classe
class Personnage : public Deplacable {
	public:
		// Constructeur
		Personnage(Carte* carte, int force);
};

} // moteur

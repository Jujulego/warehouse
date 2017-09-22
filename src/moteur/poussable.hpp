#pragma once

// Importations
#include "deplacable.hpp"

// Namespace
namespace moteur {

// Classe
class Poussable : public Deplacable {
	public:
		// Constructeur
		Poussable(Carte* carte);
};

} // moteur

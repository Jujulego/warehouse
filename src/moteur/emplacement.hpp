#pragma once

// Importations
#include "outils/coord.hpp"

#include "immuable.hpp"

// Namespace
namespace moteur {

// Classe
class Emplacement : public Immuable {
	public:
		// Constructeurs
		Emplacement(int x, int y);
		Emplacement(Coord const& c);
		
		// Méthodes
		bool a_bloc() const;
};

} // moteur

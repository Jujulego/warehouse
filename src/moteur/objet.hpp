#pragma once

// Importations
#include "outils/coord.hpp"

// Namespace
namespace moteur {

// Classe
class Objet {
	public:
		// Constructeur
		Objet(int x, int y);
		Objet(Coord const& c);
		
		// Destructeur
		virtual ~Objet() = default;
		
		// Accesseurs
		Coord const& coord() const;
	
	protected:
		// Attributs
		Coord m_coord;
};

} // moteur

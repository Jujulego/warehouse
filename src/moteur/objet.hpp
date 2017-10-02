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
		
		// Regle de 5
		Objet(Objet const& obj) = default;
		Objet(Objet&& obj)      = default;
		Objet& operator = (Objet const& obj) = default;
		Objet& operator = (Objet&& obj)      = default;
		
		// Destructeur
		virtual ~Objet() = default;
		
		// Accesseurs
		Coord const& coord() const;
	
	protected:
		// Attributs
		Coord m_coord;
};

} // moteur

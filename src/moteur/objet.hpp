#pragma once

// Importations
#include "outils/coord.hpp"

// Namespace
namespace moteur {

// Classe
class Objet {
	public:
		// Constructeur
		Objet(Coord const& c);
		
		// Destructeur
		virtual ~Objet() = default;
		
		// Méthodes
		virtual bool accessible() const; //! Indique s'il est possible de marcher sur l'objet.
		
		// Accesseurs
		Coord&       coord();
		Coord const& coord() const;
	
	private:
		// Attributs
		Coord m_coord;
};

} // moteur

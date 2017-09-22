// Importations
#include "outils/coord.hpp"

#include "objet.hpp"

// Namespace
using namespace moteur;

// Constructeur
Objet::Objet(int x, int y)   : m_coord(x, y) {};
Objet::Objet(Coord const& c) : m_coord(c) {};

// Accesseurs
Coord const& Objet::coord() const {
	return m_coord;
}

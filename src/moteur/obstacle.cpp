// Importations
#include "outils/coord.hpp"

#include "objet.hpp"
#include "obstacle.hpp"

// Namespace
using namespace moteur;

// Construteur
Obstacle::Obstacle(Coord const& c, Type type) : Objet(c),    m_type(type) {}
Obstacle::Obstacle(int x, int y,   Type type) : Objet(x, y), m_type(type) {}

// Méthodes
bool Obstacle::accessible() const {
	return false;
}

// Accesseurs
Obstacle::Type Obstacle::type() const {
	return m_type;
}

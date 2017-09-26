// Importations
#include "outils/coord.hpp"

#include "immuable.hpp"
#include "obstacle.hpp"

// Namespace
using namespace moteur;

// Construteur
Obstacle::Obstacle(Coord const& c, Type type) : Immuable(c),    m_type(type) {}
Obstacle::Obstacle(int x, int y,   Type type) : Immuable(x, y), m_type(type) {}

// Méthodes
bool Obstacle::accessible() const {
	return false;
}

std::shared_ptr<Immuable> Obstacle::copie(Carte* carte) const {
	auto pt = std::make_shared<Obstacle>(m_coord, m_type);
	pt->set(get()->copie(carte));
	
	return pt;
}

// Accesseurs
Obstacle::Type Obstacle::type() const {
	return m_type;
}

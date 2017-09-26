// Importations
#include <memory>

#include "outils/coord.hpp"

#include "chemin.hpp"
#include "noeud.hpp"

// Namespace
using namespace ia;

// Constructeur
Noeud::Noeud() : m_pere(nullptr), m_mvt(0, 0) {}
Noeud::Noeud(std::shared_ptr<Noeud> const& pere, Coord const& mvt) : m_pere(pere), m_mvt(mvt) {}

// Méthodes
Chemin Noeud::chemin() const {
	Chemin c;
	
	if (m_pere) c = m_pere->chemin();
	c.ajouter(m_mvt);
	
	return c;
}

// Accesseur
Coord Noeud::mvt() const {
	return m_mvt;
}

std::shared_ptr<Noeud> Noeud::pere() const {
	return m_pere;
}

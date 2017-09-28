// Importations
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"
#include "noeud.hpp"

// Namespace
using namespace ia;

// Constructeur
Noeud::Noeud() : m_mvt(0, 0), m_pere(nullptr) {}
Noeud::Noeud(int x, int y, std::shared_ptr<Noeud> const& pere) : m_mvt(Coord(x, y)), m_pere(pere) {}
Noeud::Noeud(Coord const& mvt, std::shared_ptr<Noeud> const& pere) : m_mvt(mvt), m_pere(pere) {}

// Méthodes
Chemin Noeud::chemin() const {
	Chemin c;
	
	if (m_pere) c = m_pere->chemin();
	c.ajouter(m_mvt);
	
	return c;
}

std::shared_ptr<moteur::Carte> Noeud::carte(std::shared_ptr<moteur::Carte> const& base, Coord& obj, int force) {
	auto carte = std::make_shared<moteur::Carte>(*base);
	chemin().appliquer(carte, obj, force);
	
	return carte;
}

// Accesseur
Coord Noeud::mvt() const {
	return m_mvt;
}

std::shared_ptr<Noeud> Noeud::pere() const {
	return m_pere;
}

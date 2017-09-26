// Importations
#include <memory>

#include "outils/coord.hpp"

#include "carte.hpp"
#include "deplacable.hpp"

// Namespace
using namespace moteur;

// Construteur
Deplacable::Deplacable(Carte* carte, int force) : Objet(-1, -1), m_carte(carte), m_force(force) {}

// Méthodes
bool Deplacable::deplacer(Coord const& vecteur, bool fake) {
	return m_carte->deplacer(coord(), vecteur, m_force, fake);
}

std::shared_ptr<Deplacable> Deplacable::copie(Carte* carte) const {
	return std::make_shared<Deplacable>(carte, m_force);
}

// Accesseur
Coord& Deplacable::coord() {
	return m_coord;
}

int Deplacable::force() const {
	return m_force;
}

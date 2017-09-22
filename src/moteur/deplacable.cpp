// Importations
#include "outils/coord.hpp"

#include "carte.hpp"
#include "deplacable.hpp"

// Namespace
using namespace moteur;

// Construteur
Deplacable::Deplacable(Carte* carte, int force) : Objet(-1, -1), m_carte(carte), m_force(force) {}

// Méthodes
bool Deplacable::deplacer(Coord const& vecteur) {
	return m_carte->deplacer(coord(), vecteur, m_force);
}

// Accesseur
Coord& Deplacable::coord() {
	return m_coord;
}

int Deplacable::force() const {
	return m_force;
}

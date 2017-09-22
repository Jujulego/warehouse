// Importations
#include "outils/coord.hpp"

#include "carte.hpp"
#include "deplacable.hpp"

// Namespace
using namespace moteur;

// Construteur
Deplacable::Deplacable(Carte* carte) : Objet(-1, -1), m_carte(carte) {}

// Méthodes
bool Deplacable::deplacer(Coord const& vecteur) {
	return m_carte->deplacer(coord(), vecteur);
}

// Accesseur
Coord& Deplacable::coord() {
	return m_coord;
}

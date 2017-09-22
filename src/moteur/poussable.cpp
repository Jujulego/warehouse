// Importations
#include "carte.hpp"
#include "poussable.hpp"

// Namespace
using namespace moteur;

// Constructeur
Poussable::Poussable(Carte* carte, int poids) : Deplacable(carte, 0), m_poids(poids) {};

// Accesseurs
int Poussable::poids() const {
	return m_poids;
}

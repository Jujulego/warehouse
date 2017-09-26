// Importations
#include <memory>

#include "carte.hpp"
#include "poussable.hpp"

// Namespace
using namespace moteur;

// Constructeur
Poussable::Poussable(Carte* carte, int poids) : Deplacable(carte, 0), m_poids(poids) {};

// Méthodes
std::shared_ptr<Deplacable> Poussable::copie(Carte* carte) const {
	return std::make_shared<Poussable>(carte, m_poids);
}

// Accesseurs
int Poussable::poids() const {
	return m_poids;
}

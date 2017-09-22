// Importations
#include <memory>

#include "outils/coord.hpp"

#include "deplacable.hpp"
#include "immuable.hpp"

// Namespace
using namespace moteur;

// Construteur
Immuable::Immuable(Coord const& c) : Objet(c) {}
Immuable::Immuable(int x, int y)   : Objet(x, y) {}

// Méthodes
bool Immuable::accessible() const {
	return m_objet == nullptr;
}

// Accesseurs
std::shared_ptr<Deplacable> Immuable::pop() {
	auto obj = m_objet;
	m_objet = nullptr;
	
	return obj;
}

std::shared_ptr<Deplacable> Immuable::get() const {
	return m_objet;
}

void Immuable::set(std::shared_ptr<Deplacable> obj) {
	obj->coord() = coord();
	m_objet = obj;
}

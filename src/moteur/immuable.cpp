// Importations
#include <memory>

#include "outils/coord.hpp"

#include "carte.hpp"
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

std::shared_ptr<Immuable> Immuable::copie(Carte* carte) const {
	auto pt = std::make_shared<Immuable>(m_coord);
	if (m_objet) pt->set(m_objet->copie(carte));
	
	return pt;
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

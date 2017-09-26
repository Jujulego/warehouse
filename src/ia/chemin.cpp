// Importations
#include <list>
#include <memory>

#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"

// Namespace
using namespace ia;

// Méthodes
void Chemin::ajouter(int x, int y) {
	ajouter(Coord(x, y));
}

void Chemin::ajouter(Coord const& c) {
	m_actions.push_back(c);
}

bool Chemin::appliquer(std::shared_ptr<moteur::Deplacable> const& obj) const {
	for (auto a : m_actions) {
		if (obj->deplacer(a)) return true;
	}
	
	return false;
}

// Itérateurs
Chemin::iterator Chemin::begin() {
	return m_actions.begin();
}

Chemin::iterator Chemin::end() {
	return m_actions.end();
}

Chemin::const_iterator Chemin::begin() const {
	return m_actions.cbegin();
}

Chemin::const_iterator Chemin::end() const {
	return m_actions.cend();
}

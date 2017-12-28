// Importations
#include <list>
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"

// Namespace
using namespace ia;

// Méthodes
void Chemin::ajouter(int x, int y) {
	if (x != 0 && y != 0) ajouter(Coord(x, y));
}

void Chemin::ajouter(Coord const& c) {
	if (c != ORIGINE) m_actions.push_back(c);
}

void Chemin::ajouter(Chemin const& c) {
	for (auto a : c) {
		ajouter(a);
	}
}

Coord Chemin::pop() {
	Coord c = m_actions.front();
	m_actions.pop_front();
	return c;
}

int Chemin::longueur() const {
	return m_actions.size();
}

Coord Chemin::appliquer(Coord coord) const {
	for (auto a : m_actions) {
		coord += a;
	}

	return coord;
}

bool Chemin::appliquer(std::shared_ptr<moteur::Deplacable> const& obj) const {
	for (auto a : m_actions) {
		if (obj->deplacer(a)) return true;
	}

	return false;
}

bool Chemin::appliquer(std::shared_ptr<moteur::Carte> const& carte, Coord& obj, int force) const {
	for (auto a : m_actions) {
		if (carte->deplacer(obj, a, force)) return true;
		obj += a;
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

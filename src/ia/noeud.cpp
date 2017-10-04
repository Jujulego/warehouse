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
Noeud::Noeud() : m_pere(nullptr) {}
Noeud::Noeud(Chemin const& chemin, std::shared_ptr<Noeud> const& pere) : m_chemin(chemin), m_pere(pere) {}

Noeud::Noeud(int x, int y, std::shared_ptr<Noeud> const& pere) : m_pere(pere) {
	m_chemin.ajouter(x, y);
}

Noeud::Noeud(Coord const& mvt, std::shared_ptr<Noeud> const& pere) : m_pere(pere) {
	m_chemin.ajouter(mvt);
}

// Méthodes
Chemin Noeud::chemin_complet() const {
	Chemin c;
	
	if (m_pere) c = m_pere->chemin_complet();
	c.ajouter(m_chemin);
	
	return c;
}

std::shared_ptr<moteur::Carte> Noeud::carte(std::shared_ptr<moteur::Carte> const& base, Coord& obj, int force) {
	auto carte = std::make_shared<moteur::Carte>(*base);
	chemin_complet().appliquer(carte, obj, force);
	
	return carte;
}

// Accesseur
Chemin Noeud::chemin() const {
	return m_chemin;
}

std::shared_ptr<Noeud> Noeud::pere() const {
	return m_pere;
}

// Importations
#include <memory>

#include "moteur/carte.hpp"
#include "outils/coord.hpp"

#include "chemin.hpp"
#include "noeud.hpp"

// Namespace
using namespace ia;

// Constructeur
Noeud::Noeud() : m_mvt(0, 0), m_pere(nullptr) {}
Noeud::Noeud(int x, int y, std::shared_ptr<Noeud> const& pere) : m_mvt(Coord(x, y)), m_pere(pere) {}
Noeud::Noeud(Coord const& mvt, std::shared_ptr<Noeud> const& pere) : m_mvt(mvt), m_pere(pere) {}

// Méthodes
Chemin Noeud::chemin() const {
	Chemin c;
	
	if (m_pere) c = m_pere->chemin();
	c.ajouter(m_mvt);
	
	return c;
}

std::shared_ptr<moteur::Carte> Noeud::carte(std::shared_ptr<moteur::Carte> const& base, Coord& obj, int force) {
	// Check calcul
	if (m_carte) {
		obj = m_coord;
		return m_carte;
	}
	
	// Copie de la carte
	if (m_pere) {
		m_carte = std::make_shared<moteur::Carte>(*(m_pere->carte(base, obj, force)));
	} else {
		m_carte = std::make_shared<moteur::Carte>(*base);
	}
	
	// Application du mouvement
	m_carte->deplacer(obj, m_mvt, force);
	m_coord = obj += m_mvt;
	
	return m_carte;
}

// Accesseur
Coord Noeud::mvt() const {
	return m_mvt;
}

std::shared_ptr<Noeud> Noeud::pere() const {
	return m_pere;
}

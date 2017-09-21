// Importations
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "outils.hpp"
#include "outils/coord.hpp"

#include "carte.hpp"
#include "objet.hpp"
#include "obstacle.hpp"

// Namespace
using namespace moteur;

// Constructeur
Carte::Carte(int tx, int ty) : m_tx(tx), m_ty(ty) {
	// Remplissage du tableau
	for (int x = 0; x < tx; ++x) { for (int y = 0; y < ty; ++y) {
		m_objets.push_back(std::make_shared<Objet>(x, y));
	}}
}

// Opérateurs
std::shared_ptr<Objet>& Carte::operator [] (Coord const& c) {
	return m_objets[c.x() * m_ty + c.y()];
}

std::shared_ptr<Objet> const& Carte::operator [] (Coord const& c) const {
	return m_objets[c.x() * m_ty + c.y()];
}

// Méthodes statiques
Carte Carte::charger(std::string const& fichier) {
	// Ouverture du fichier
	std::ifstream ifs(fichier);
	
	// Création de la carte
	unsigned tx, ty;
	ifs >> tx; ifs >> ty;
	
	Carte carte(tx, ty);
	
	// Ajout des blocs
	std::string buf;
	std::getline(ifs, buf);
	
	for (unsigned y = 0; y < ty; ++y) {
		// Lecture du fichier
		std::getline(ifs, buf);
		
		// Création des blocs
		for (unsigned x = 0; x < min(buf.size(), tx); ++x) {
			switch (buf[x]) {
			case '#':
				carte.set(x, y, std::make_shared<Obstacle>(x, y, Obstacle::mur));
				break;
			
			default:
				carte.set(x, y, std::make_shared<Objet>(x, y));
			}
		}
	}
	
	return carte;
}

// Méthodes
void Carte::set(Coord const& c, std::shared_ptr<Objet> obj) {
	set(c.x(), c.y(), obj);
}

void Carte::set(int x, int y, std::shared_ptr<Objet> obj) {
	// Gardiens
	if ((x < 0) || (x >= m_tx)) return;
	if ((y < 0) || (y >= m_ty)) return;
	
	// Modif
	m_objets[x * m_ty + y] = obj;
}

// Accesseurs
int Carte::taille_x() const {
	return m_tx;
}

int Carte::taille_y() const {
	return m_ty;
}

// Iterateurs
Carte::iterator Carte::begin() { return m_objets.begin(); }
Carte::iterator Carte::end()   { return m_objets.end();   }

Carte::const_iterator Carte::begin() const { return m_objets.cbegin(); }
Carte::const_iterator Carte::end()   const { return m_objets.cend();   }

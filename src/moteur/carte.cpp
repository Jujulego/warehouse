// Importations
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "outils.hpp"
#include "outils/coord.hpp"

#include "carte.hpp"
#include "emplacement.hpp"
#include "immuable.hpp"
#include "obstacle.hpp"
#include "poussable.hpp"
#include "sortie.hpp"

// Namespace
using namespace moteur;

// Constructeur
Carte::Carte(int tx, int ty) : m_tx(tx), m_ty(ty) {
	// Remplissage du tableau
	for (int x = 0; x < tx; ++x) { for (int y = 0; y < ty; ++y) {
		m_objets.push_back(std::make_shared<Immuable>(x, y));
	}}
}

// Opérateurs
std::shared_ptr<Immuable>& Carte::operator [] (Coord const& c) {
	return m_objets[c.x() * m_ty + c.y()];
}

std::shared_ptr<Immuable> const& Carte::operator [] (Coord const& c) const {
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
			
			case 'E':
				carte.set(x, y, std::make_shared<Emplacement>(x, y));
				break;
			
			case 'S':
				carte.set(x, y, std::make_shared<Sortie>(x, y));
				break;
			
			default:
				carte.set(x, y, std::make_shared<Immuable>(x, y));
			}
		}
	}
	
	return carte;
}

// Méthodes
bool Carte::coord_valides(Coord const& c) const {
	return coord_valides(c.x(), c.y());
}

bool Carte::coord_valides(int x, int y) const {
	return (x >= 0) && (x < m_tx) && (y >= 0) && (y < m_ty);
}

void Carte::set(Coord const& c, std::shared_ptr<Objet> obj) {
	set(c.x(), c.y(), obj);
}

void Carte::set(int x, int y, std::shared_ptr<Objet> obj) {
	// Gardiens
	if (!coord_valides(x, y)) return;
	
	// Modif
	auto pt = std::dynamic_pointer_cast<Immuable>(obj);
	if (pt) {
		m_objets[x * m_ty + y] = pt;
	} else {
		m_objets[x * m_ty + y]->set(std::dynamic_pointer_cast<Deplacable>(obj));
	}
}

bool Carte::deplacer(Coord const& c, Coord const& vecteur, int force) {
	// Check coordonnees
	Coord nc = c + vecteur;
	if (!coord_valides(c))  return true;
	if (!coord_valides(nc)) return true;
	
	// Check présence d'objet déplacable
	auto obj = get<Deplacable>(c);
	if (!obj) return true;
	
	// Check accessiblité
	if (!get<Immuable>(nc)->accessible()) {
		// Y a 1 truc ...
		auto pobj = get<Poussable>(nc);
		if (!pobj || (force < pobj->poids())) return true;
		
		// ... qu'on peut (peut-être) pousser !
		if (deplacer(nc, vecteur, force - pobj->poids())) return true; // ou pas ;)
	}
	
	// Mouvement !
	get<Immuable>(nc)->set(get<Immuable>(c)->pop());
	
	return false;
}

bool Carte::test_fin() const {
	bool ok = true;
	
	// Parcours des cases
	for (auto obj : *this) {
		auto empl   = std::dynamic_pointer_cast<Emplacement>(obj);
		auto sortie = std::dynamic_pointer_cast<Sortie>(obj);
		
		if (empl) {
			ok &= empl->a_bloc();
		} else if (sortie) {
			ok &= sortie->a_pers();
		}
	}
	
	return ok;
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

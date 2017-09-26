// Importations
#include <memory>

#include "outils/coord.hpp"

#include "personnage.hpp"
#include "sortie.hpp"

// Namespace
using namespace moteur;

// Constructeur
Sortie::Sortie(int x, int y)   : Immuable(x, y) {};
Sortie::Sortie(Coord const& c) : Immuable(c) {};

// Méthodes
bool Sortie::a_pers() const {
	return std::dynamic_pointer_cast<Personnage>(get()) != nullptr;
}

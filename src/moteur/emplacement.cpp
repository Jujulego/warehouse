// Importations
#include <memory>

#include "emplacement.hpp"
#include "poussable.hpp"

// Namespace
using namespace moteur;

// Constructeurs
Emplacement::Emplacement(int x, int y) : Immuable(x, y) {}
Emplacement::Emplacement(Coord const& c) : Immuable(c) {}

// Méthodes
bool Emplacement::a_bloc() const {
	return std::dynamic_pointer_cast<Poussable>(get()) != nullptr;
}

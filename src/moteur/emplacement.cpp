// Importations
#include <memory>

#include "outils/coord.hpp"

#include "emplacement.hpp"
#include "immuable.hpp"
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

std::shared_ptr<Immuable> Emplacement::copie(Carte* carte) const {
	auto pt = std::make_shared<Emplacement>(m_coord);
	if (get()) pt->set(get()->copie(carte));
	
	return pt;
}


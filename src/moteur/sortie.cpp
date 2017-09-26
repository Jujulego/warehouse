// Importations
#include <memory>

#include "outils/coord.hpp"

#include "immuable.hpp"
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

std::shared_ptr<Immuable> Sortie::copie(Carte* carte) const {
	auto pt = std::make_shared<Sortie>(m_coord);
	if (get()) pt->set(get()->copie(carte));
	
	return pt;
}

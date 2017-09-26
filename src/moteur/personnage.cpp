// Importations
#include <iostream>
#include <memory>

#include "outils/style.hpp"

#include "carte.hpp"
#include "deplacable.hpp"
#include "personnage.hpp"

// Namespace
using namespace moteur;

// Constructeur
Personnage::Personnage(Carte* carte, int force) : Deplacable(carte, force) {
	if (force <= 0) std::cout << style::warning << "Attention création d'un personnage de force nulle !" << style::defaut << std::endl;
}

// Méthodes
std::shared_ptr<Deplacable> Personnage::copie(Carte* carte) const {
	return std::make_shared<Personnage>(carte, force());
}

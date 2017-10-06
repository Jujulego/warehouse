#pragma once

// Importations
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"

#include "chemin.hpp"
#include "ia.hpp"

// Namespace
namespace ia {

// Classe
class Solveur2 : public IA {
	public:
		// Constructeur
		Solveur2(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj);
		
		// Méthodes
		virtual Chemin resoudre() override;
};

} // ia

#pragma once

// Importations
#include "immuable.hpp"

#include "outils/coord.hpp"

// Namespace
namespace moteur {

// Classe
class Sortie : public Immuable {
	public:
		// Constructeur
		Sortie(int x, int y);
		Sortie(Coord const& c);
		
		// Méthodes
		bool a_pers() const;
		virtual std::shared_ptr<Immuable> copie(Carte* carte) const override;
};

} // moteur

#pragma once

// Importations
#include <memory>

#include "outils/coord.hpp"

#include "immuable.hpp"

// Namespace
namespace moteur {

// Classe
class Obstacle : public Immuable {
	public:
		// Enumération
		enum Type { mur };
		
		// Construteur
		Obstacle(Coord const& c, Type type);
		Obstacle(int x, int y,   Type type);
		
		// Méthodes
		virtual bool accessible() const override;
		virtual std::shared_ptr<Immuable> copie(Carte* carte) const override;
		
		// Accesseurs
		Type type() const;
	
	private:
		// Attributs
		Type m_type;
};

} // moteur

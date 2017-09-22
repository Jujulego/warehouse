#pragma once

// Importations
#include <memory>

#include "outils/coord.hpp"

#include "deplacable.hpp"
#include "objet.hpp"

// Namespace
namespace moteur {

// Classe
class Immuable : public Objet {
	public:
		// Construteur
		Immuable(Coord const& c);
		Immuable(int x, int y);
		
		// Méthodes
		virtual bool accessible() const;
		
		// Accesseurs
		std::shared_ptr<Deplacable> pop();
		std::shared_ptr<Deplacable> get() const;
		void set(std::shared_ptr<Deplacable> obj);
	
	private:
		// Attributs
		std::shared_ptr<Deplacable> m_objet = nullptr;
};

} // moteur


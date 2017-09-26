#pragma once

// Importations
#include <memory>

#include "outils/coord.hpp"

#include "deplacable.hpp"
#include "objet.hpp"

// Namespace
namespace moteur {

class Carte;

// Classe
class Immuable : public Objet {
	public:
		// Construteur
		Immuable(Coord const& c);
		Immuable(int x, int y);
		
		// Destructeur
		virtual ~Immuable() = default;
		
		// Méthodes
		virtual bool accessible() const;
		virtual std::shared_ptr<Immuable> copie(Carte* carte) const;
		
		// Accesseurs
		std::shared_ptr<Deplacable> pop();
		std::shared_ptr<Deplacable> get() const;
		void set(std::shared_ptr<Deplacable> obj);
	
	private:
		// Attributs
		std::shared_ptr<Deplacable> m_objet = nullptr;
};

} // moteur


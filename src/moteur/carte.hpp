#pragma once

// Importations
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "outils/coord.hpp"

#include "objet.hpp"

// Namespace
namespace moteur {

// Classe
class Carte {
	public:
		// Alias
		using iterator       = std::vector<std::shared_ptr<Objet>>::iterator;
		using const_iterator = std::vector<std::shared_ptr<Objet>>::const_iterator;
		
		// Constructeur
		Carte(int tx, int ty);
		
		// Opérateurs
		std::shared_ptr<Objet>&       operator [] (Coord const& c);       //! Accès aux objets : carte[Coord(5, 5)] => pt sur Objet
		std::shared_ptr<Objet> const& operator [] (Coord const& c) const;
		
		// Méthodes statiques
		static Carte charger(std::string const& fichier);
		
		// Méthodes
		template<class T = Objet>
		std::shared_ptr<T> get(Coord const& c) const {
			return get<T>(c.x(), c.y());
		}
		
		template<class T = Objet>
		std::shared_ptr<T> get(int x, int y) const {
			// Assertion : ne compile QUE si T hérite de Objet
			static_assert(std::is_base_of<Objet,T>::type::value, "T doit hériter de moteur::Objet !");
			
			// Gardiens
			if ((x < 0) || (x >= m_tx)) return nullptr;
			if ((y < 0) || (y >= m_ty)) return nullptr;
			
			// Acces :
			return std::dynamic_pointer_cast<T>(m_objets[x * m_ty + y]);
		}
		
		void set(Coord const& c, std::shared_ptr<Objet> obj);
		void set(int x, int y,   std::shared_ptr<Objet> obj);
		
		// Accesseurs
		int taille_x() const;
		int taille_y() const;
		
		// Itérateurs
		iterator begin();
		iterator end();
		
		const_iterator begin() const;
		const_iterator end()   const;
	
	private:
		// Attributs
		int m_tx, m_ty;
		std::vector<std::shared_ptr<Objet>> m_objets;
};

} // moteur

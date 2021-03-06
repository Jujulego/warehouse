#pragma once

// Importations
#include <memory>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

#include "outils/coord.hpp"

#include "immuable.hpp"
#include "deplacable.hpp"
#include "objet.hpp"
#include "personnage.hpp"

// Namespace
namespace moteur {

// Classe
class Carte {
	public:
		// Alias
		using iterator       = std::vector<std::shared_ptr<Immuable>>::iterator;
		using const_iterator = std::vector<std::shared_ptr<Immuable>>::const_iterator;

		// Métafonctions
		template<class T>
		struct est_immuable : std::enable_if<std::is_base_of<Immuable,T>::type::value,std::shared_ptr<T>> {};

		template<class T>
		struct est_deplacable : std::enable_if<std::is_base_of<Deplacable,T>::type::value,std::shared_ptr<T>> {};

		// Constructeur
		Carte(int tx, int ty);
		Carte(Carte const& carte);

		// Opérateurs
		Carte& operator = (Carte const& carte);

		std::shared_ptr<Immuable>&       operator [] (Coord const& c);       //! Accès aux objets : carte[Coord(5, 5)] => pt sur Objet
		std::shared_ptr<Immuable> const& operator [] (Coord const& c) const;

		// Méthodes statiques
		static std::shared_ptr<Carte> charger(std::string const& fichier);

		// Méthodes
		bool coord_valides(int x, int y)   const;
		bool coord_valides(Coord const& c) const;

		template<class T = Objet>
		std::shared_ptr<T> get(Coord const& c) const {
			return get<T>(c.x(), c.y());
		}

		template<class T>
		typename est_immuable<T>::type get(int x, int y) const {
			// Gardien
			if (!coord_valides(x, y)) return nullptr;

			// Acces :
			return std::dynamic_pointer_cast<T>(m_objets[m_hash(x, y)]);
		}

		template<class T>
		typename est_deplacable<T>::type get(int x, int y) const {
			return std::dynamic_pointer_cast<T>(get<Immuable>(x, y)->get());
		}

		template<class T>
		std::list<std::shared_ptr<T>> liste() const {
			std::list<std::shared_ptr<T>> liste;

			for (int x = 0; x < m_tx; ++x) { for (int y = 0; y < m_ty; ++y) {
				auto pt = get<T>(x, y);

				if (pt) liste.push_back(pt);
			}}

			return liste;
		}

		void set(Coord const& c, std::shared_ptr<Objet> obj);
		void set(int x, int y,   std::shared_ptr<Objet> obj);

		// interaction jeu
		bool deplacer(Coord const& c, Coord const& vecteur, int force, bool fake = false);
		bool deplacer(Coord const& c, Coord const& vecteur, int force, int& push, bool fake = false);
		bool test_fin() const;

		// Accesseurs
		int taille_x() const;
		int taille_y() const;
		std::shared_ptr<Personnage> personnage() const;

		// Itérateurs
		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end()   const;

	private:
		// Attributs
		int m_tx, m_ty;
		std::hash<Coord> m_hash;
		std::vector<std::shared_ptr<Immuable>> m_objets;
};

} // moteur

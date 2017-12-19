#pragma once

// Importations
#include <functional>

// Classe
class Coord {
	public:
		// Constructeurs
		Coord() = default;
		Coord(int x, int y);

		// Opérateurs
		Coord  operator +  () const;
		Coord  operator -  () const;
		Coord  operator +  (Coord const& c) const;
		Coord  operator -  (Coord const& c) const;
		Coord  operator *  (int k) const;
		Coord  operator /  (int k) const;
		Coord& operator += (Coord const& c);
		Coord& operator -= (Coord const& c);
		Coord& operator *= (int k);
		Coord& operator /= (int k);

		bool operator == (Coord const& c) const;
		bool operator != (Coord const& c) const;

		// Accesseurs
		int& x();
		int& y();
		int  x() const;
		int  y() const;

		void set(int x, int y);

	private:
		// Attributs
		int m_x = 0;
		int m_y = 0;
};

// Opérateurs externes
Coord operator * (int k, Coord const& c);

// Directions
static const Coord ORIGINE(0,  0);
static const Coord HAUT(   0, -1);
static const Coord DROITE( 1,  0);
static const Coord BAS(    0,  1);
static const Coord GAUCHE(-1,  0);

// Hash
namespace std {

template<>
class hash<Coord> {
	public:
		// Constructeur
		hash(size_t factx, size_t minx = 0, size_t miny = 0) : m_factx(factx), m_minx(-minx), m_miny(-miny) {}

		// Opérateur d'appel
		size_t operator () (Coord const& c) const {
			return (c.x() + m_minx) * m_factx + (c.y() + m_miny);
		}

		size_t operator () (int x, int y) const {
			return (x + m_minx) * m_factx + (y + m_miny);
		}

		// Méthodes
		Coord unhash(size_t h) const {
			return Coord((h / m_factx) - m_minx, ((h - m_miny) % m_factx));
		}

	private:
		// Attributs
		size_t m_factx;
		size_t m_minx;
		size_t m_miny;
};

template<>
class less<Coord> {
	public:
		// Constructeur
		less(std::hash<Coord> const& hash) : m_hash(hash) {}
		less(size_t factx, size_t minx = 0, size_t miny = 0) : m_hash(factx, minx, miny) {}

		// Opérateur d'appel
		bool operator () (Coord const& c1, Coord const& c2) const {
			return m_hash(c1) < m_hash(c2);
		}

	private:
		// Attributs
		std::hash<Coord> m_hash;
};

}

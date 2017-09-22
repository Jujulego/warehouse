#pragma once

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
	
	private:
		// Attributs
		int m_x = 0;
		int m_y = 0;
};

// Opérateurs externes
Coord operator * (int k, Coord const& c);

// Directions
static const Coord HAUT(   0, -1);
static const Coord DROITE( 1,  0);
static const Coord BAS(    0,  1);
static const Coord GAUCHE(-1,  0);

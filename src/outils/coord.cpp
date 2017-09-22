// Importations
#include "coord.hpp"

// Constructeur
Coord::Coord(int x, int y) : m_x(x), m_y(y) {}

// Opérateurs
Coord Coord::operator + () const {
	return *this;
}

Coord Coord::operator - () const {
	return Coord(-m_x, -m_y);
}

Coord Coord::operator + (Coord const& c) const {
	return Coord(m_x + c.m_x, m_y + c.m_y);
}

Coord Coord::operator - (Coord const& c) const {
	return Coord(m_x - c.m_x, m_y - c.m_y);
}

Coord Coord::operator * (int k) const {
	return Coord(k * m_x, k * m_y);
}

Coord Coord::operator / (int k) const {
	return Coord(m_x / k, m_y / k);
}

Coord& Coord::operator += (Coord const& c) {
	m_x += c.m_x;
	m_y += c.m_y;
	
	return *this;
}

Coord& Coord::operator -= (Coord const& c) {
	m_x -= c.m_x;
	m_y -= c.m_y;
	
	return *this;
}

Coord& Coord::operator *= (int k) {
	m_x *= k;
	m_y *= k;
	
	return *this;
}

Coord& Coord::operator /= (int k) {
	m_x /= k;
	m_y /= k;
	
	return *this;
}

bool Coord::operator == (Coord const& c) const {
	return (m_x == c.x()) && (m_y == c.y());
}

bool Coord::operator != (Coord const& c) const {
	return (m_x != c.x()) || (m_y != c.y());
}

// Accesseurs
int& Coord::x() {
	return m_x;
}

int& Coord::y() {
	return m_y;
}

int Coord::x() const {
	return m_x;
}

int Coord::y() const {
	return m_y;
}

// Opérateurs externes
Coord operator * (int k, Coord const& c) {
	return c * k;
}

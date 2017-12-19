// Importations
#include <stdexcept>

#include "infini.hpp"

// Opérateurs
// - cast
Infini::operator bool () const {
	return true;
}

bool Infini::operator ! () const {
	return false;
}

// - arithmétiques
Infini Infini::operator - () const {
	Infini inf;
	inf.neg = !neg;

	return inf;
}

Infini Infini::operator + (Infini const& inf) const {
	Infini r(*this);
	r += inf;

	return r;
}

Infini Infini::operator - (Infini const& inf) const {
	Infini r(*this);
	r -= inf;

	return r;
}

Infini Infini::operator * (Infini const& inf) const {
	Infini r(*this);
	r.neg *= inf;

	return r;
}

Infini& Infini::operator += (Infini const& inf) {
	if (neg ^ inf.neg) {
		throw std::domain_error("cannot add inf to -inf");
	}

	return *this;
}

Infini& Infini::operator -= (Infini const& inf) {
	if (neg == inf.neg) {
		throw std::domain_error("cannot substract inf to inf");
	}

	return *this;
}

Infini& Infini::operator *= (Infini const& inf) {
	neg ^= inf.neg;

	return *this;
}

// - comparaison
bool Infini::operator <  (Infini const& inf) const {
	return neg && !inf.neg;
}

bool Infini::operator <= (Infini const& inf) const {
	return neg && !inf.neg;
}

bool Infini::operator == (Infini const& inf) const {
	return &inf == this;
}

bool Infini::operator != (Infini const& inf) const {
	return &inf != this;
}

bool Infini::operator >= (Infini const& inf) const {
	return !neg && inf.neg;
}

bool Infini::operator >  (Infini const& inf) const {
	return !neg && inf.neg;
}

// Accesseur
bool Infini::est_pos() const {
	return !neg;
}

bool Infini::est_neg() const {
	return neg;
}

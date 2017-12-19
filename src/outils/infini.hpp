#pragma once

// Importations
#include <ostream>
#include <stdexcept>

// Classe
class Infini {
	private:
		// Attributs
		bool neg = false;

	public:
		// Opérateurs
		// - cast
		operator bool () const;
		bool operator ! () const;

		// - arithmétiques
		Infini operator - () const;

		Infini operator + (Infini const& inf) const;
		Infini operator - (Infini const& inf) const;
		Infini operator * (Infini const& inf) const;

		Infini& operator += (Infini const& inf);
		Infini& operator -= (Infini const& inf);
		Infini& operator *= (Infini const& inf);

		// - comparaison
		bool operator <  (Infini const& inf) const;
		bool operator <= (Infini const& inf) const;
		bool operator == (Infini const& inf) const;
		bool operator != (Infini const& inf) const;
		bool operator >= (Infini const& inf) const;
		bool operator >  (Infini const& inf) const;

		// - affichage
		friend std::ostream& operator << (std::ostream& stream, Infini const& inf) {
			return stream << (inf.neg ? "-inf" : "inf");
		}

		// Accesseur
		bool est_pos() const;
		bool est_neg() const;
};

static const Infini INFINI;

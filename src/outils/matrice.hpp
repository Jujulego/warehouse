#pragma once

// Imporation
#include <array>
#include <iterator>

#include "coord.hpp"

// Classe
template<class Int, size_t LIG, size_t COL = LIG>
class Matrice {
	private:
		// Attributs
		std::hash<Coord> hash;
		std::array<Int, LIG * COL> m_matrice;

	public:
		// Alias
		using value_type       = Int;
		using reference        = Int&;
		using const_reference  = Int const&;
		using pointer          = Int*;
		using const_pointer    = Int const*;
		using iterator         = typename std::array<Int, LIG * COL>::iterator;
		using const_iterator   = typename std::array<Int, LIG * COL>::const_iterator;
		using size_type        = size_t;
		using difference_type  = ptrdiff_t;

		// Sous-classes
		class column_iterator {
			private:
				// Attributs
				iterator m_it;

			public:
				// Alias
				using value_type        = Matrice::value_type;
				using reference         = Matrice::reference;
				using difference_type   = Matrice::difference_type;
				using iterator_category = std::random_access_iterator_tag;

				// Constructeurs
				column_iterator() : m_it() {};
				column_iterator(iterator const& it) : m_it(it) {};

				// Opérateurs
				// - déférenciation
				reference operator *  () const { return *m_it; }
				pointer   operator -> () const { return m_it.operator ->(); }
				reference operator [] (Matrice::size_type v) const { return m_it[v]; }

				// - comparaison
				bool operator <  (column_iterator const& cit) const { return m_it <  cit.m_it; };
				bool operator <= (column_iterator const& cit) const { return m_it <= cit.m_it; };
				bool operator == (column_iterator const& cit) const { return m_it == cit.m_it; };
				bool operator != (column_iterator const& cit) const { return m_it != cit.m_it; };
				bool operator >= (column_iterator const& cit) const { return m_it >= cit.m_it; };
				bool operator >  (column_iterator const& cit) const { return m_it >  cit.m_it; };

				// - incrémentation & décrémentation
				column_iterator& operator ++ () { m_it += COL; return *this; }
				column_iterator& operator -- () { m_it -= COL; return *this; }

				column_iterator operator ++ (int) {
					column_iterator tmp(*this);
					++(*this);
					return tmp;
				}

				column_iterator operator -- (int) {
					column_iterator tmp(*this);
					--(*this);
					return tmp;
				}

				// - assignation
				column_iterator& operator += (Matrice::size_type v) { m_it += v * COL; return *this; }
				column_iterator& operator -= (Matrice::size_type v) { m_it -= v * COL; return *this; }

				// - arithmétique
				column_iterator operator + (Matrice::size_type v) const { column_iterator r(*this); r += v; return r; };
				column_iterator operator - (Matrice::size_type v) const { column_iterator r(*this); r -= v; return r; };

				friend column_iterator operator + (Matrice::size_type v, column_iterator const& it) {
					column_iterator r(it);
					r += v;
					return r;
				};

				difference_type operator - (column_iterator cit) const {
					return (m_it - cit.m_it) / COL;
				};
		};

		class const_column_iterator {
			private:
				// Attributs
				const_iterator m_it;

			public:
				// Alias
				using value_type        = Matrice::value_type;
				using reference         = Matrice::const_reference;
				using difference_type   = Matrice::difference_type;
				using iterator_category = std::random_access_iterator_tag;

				// Constructeurs
				const_column_iterator() : m_it() {};
				const_column_iterator(const_iterator const& it) : m_it(it) {};
				const_column_iterator(column_iterator const& cit) : m_it(cit.m_it) {};

				// Opérateurs
				// - déférenciation
				reference operator *  () const { return *m_it; }
				pointer   operator -> () const { return m_it.operator ->(); }
				reference operator [] (Matrice::size_type v) const { return m_it[v]; }

				// - comparaison
				bool operator <  (const_column_iterator const& cit) const { return m_it <  cit.m_it; };
				bool operator <= (const_column_iterator const& cit) const { return m_it <= cit.m_it; };
				bool operator == (const_column_iterator const& cit) const { return m_it == cit.m_it; };
				bool operator != (const_column_iterator const& cit) const { return m_it != cit.m_it; };
				bool operator >= (const_column_iterator const& cit) const { return m_it >= cit.m_it; };
				bool operator >  (const_column_iterator const& cit) const { return m_it >  cit.m_it; };

				// - incrémentation & décrémentation
				const_column_iterator& operator ++ () { m_it += COL; return *this; }
				const_column_iterator& operator -- () { m_it -= COL; return *this; }

				const_column_iterator operator ++ (int) {
					const_column_iterator tmp(*this);
					++(*this);
					return tmp;
				}

				const_column_iterator operator -- (int) {
					const_column_iterator tmp(*this);
					--(*this);
					return tmp;
				}

				// - assignation
				const_column_iterator& operator += (Matrice::size_type v) { m_it += v * COL; return *this; }
				const_column_iterator& operator -= (Matrice::size_type v) { m_it -= v * COL; return *this; }

				// - arithmétique
				const_column_iterator operator + (Matrice::size_type v) const { const_column_iterator r(*this); r += v; return r; };
				const_column_iterator operator - (Matrice::size_type v) const { const_column_iterator r(*this); r -= v; return r; };

				friend const_column_iterator operator + (Matrice::size_type v, const_column_iterator const& it) {
					const_column_iterator r(it);
					r += v;
					return r;
				};

				difference_type operator - (const_column_iterator cit) const {
					return (m_it - cit.m_it) / COL;
				};
		};

		template<class It, class CIt, size_t N>
		class part {
			private:
				// Attributs
				It m_deb;

			public:
				// Alias
				using value_type       = Matrice::value_type;
				using reference        = Matrice::reference;
				using const_reference  = Matrice::const_reference;
				using iterator         = It;
				using const_iterator   = CIt;
				using size_type        = Matrice::size_type;
				using difference_type  = Matrice::difference_type;

				// Constructeur
				part(It const& deb) : m_deb(deb) {};

				// Opérateurs
				// - accès éléments
				reference operator [] (size_type const& i) {
					return *(m_deb + i);
				}

				const_reference operator [] (size_type const& i) const {
					return *(const_iterator(m_deb) + i);
				}

				// Méthodes
				size_type size() const {
					return N;
				}

				void fill(Int const& v) {
					for (int i = 0; i < size(); ++i) (*this)[i] = v;
				}

				// Itérateurs
				iterator begin() { return m_deb; }
				iterator end()   { return m_deb + N; }

				const_iterator begin() const { return const_iterator(m_deb); }
				const_iterator end()   const { return const_iterator(m_deb + N); }
		};

		template<class CIt, size_t N>
		class const_part {
			private:
				// Attributs
				CIt m_deb;

			public:
				// Alias
				using value_type       = Matrice::value_type;
				using const_reference  = Matrice::const_reference;
				using const_iterator   = CIt;
				using size_type        = Matrice::size_type;
				using difference_type  = Matrice::difference_type;

				// Constructeurs
				template<class It>
				const_part(part<It,CIt,N> const& p) : m_deb(p.m_deb) {};
				const_part(CIt const& deb) : m_deb(deb) {};

				// Opérateurs
				// - accès éléments
				const_reference operator [] (size_type const& c) const {
					return *(m_deb + c);
				}

				// Méthodes
				size_type size() const {
					return N;
				}

				// Itérateurs
				const_iterator begin() const { return m_deb; }
				const_iterator end()   const { return m_deb + N; }
		};

		// Constructeur
		Matrice() : hash(COL) {};
		Matrice(std::array<Int, LIG * COL> const& matrice) : hash(COL), m_matrice(matrice) {};

		// Opérateurs
		// - accès élément
		reference operator [] (Coord const& c) { // (lig, col)
			return m_matrice[hash(c)];
		}

		const_reference operator [] (Coord const& c) const { // (lig, col)
			return m_matrice[hash(c)];
		}

		// - arithmétique
		Matrice operator - () const {
			Matrice r;

			for (int i = 0; i < size(); ++i) {
				r.m_matrice[i] = -m_matrice[i];
			}

			return r;
		}

		Matrice operator + (Matrice m) const { m += (*this); return m; }
		Matrice operator - (Matrice m) const { m -= (*this); return m; }

		Matrice operator / (Int const& lambda) const { Matrice r(*this); r /= lambda; return r; }
		Matrice operator * (Int const& lambda) const { Matrice r(*this); r *= lambda; return r; }
		friend Matrice operator * (Int const& lambda, Matrice m) { m *= lambda; return m; }

		// - assignation
		Matrice& operator += (Matrice const& m) {
			for (int i = 0; i < size(); ++i) {
				m_matrice[i] += m.m_matrice[i];
			}

			return *this;
		}

		Matrice& operator -= (Matrice const& m) {
			for (int i = 0; i < size(); ++i) {
				m_matrice[i] -= m.m_matrice[i];
			}

			return *this;
		}

		Matrice& operator *= (Int const& lambda) {
			for (Int& v : *this) {
				v *= lambda;
			}

			return *this;
		}

		Matrice& operator /= (Int const& lambda) {
			for (Int& v : *this) {
				v /= lambda;
			}

			return *this;
		}

		// Méthodes
		size_type size() const {
			return LIG * COL;
		}

		size_type nb_lig() const {
			return LIG;
		}

		size_type nb_col() const {
			return COL;
		}

		void fill(Int const& val) {
			m_matrice.fill(val);
		}

		auto colonne(size_type c) {
			return part<column_iterator,const_column_iterator,LIG>(begin() + c);
		}

		auto colonne(size_type c) const {
			return const_part<const_column_iterator,LIG>(begin() + c);
		}

		auto ligne(size_type l) {
			return part<iterator,const_iterator,COL>(begin() + (l * COL));
		}

		auto ligne(size_type l) const {
			return const_part<const_iterator,COL>(begin() + (l * COL));
		}

		// Itérateurs
		iterator begin() { return m_matrice.begin(); }
		iterator end()   { return m_matrice.end(); }

		const_iterator begin() const { return m_matrice.cbegin(); }
		const_iterator end()   const { return m_matrice.cend(); }
};

// Produit matriciel
template<class Int, size_t LIG, size_t LIGCOL, size_t COL>
Matrice<Int,LIG,COL> operator * (Matrice<Int,LIG,LIGCOL> const& m1, Matrice<Int,LIGCOL,COL> const& m2) {
	Matrice<Int,LIG,COL> r;
	r.fill(0);

	for (int l = 0; l < LIG; ++l) {
		for (int c = 0; c < COL; ++c) {
			Coord co(l, c);

			for (int i = 0; i < LIGCOL; ++i) {
				r[co] += m1[Coord(l, i)] * m2[Coord(i, c)];
			}
		}
	}

	return r;
}

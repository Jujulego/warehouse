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
		class line_iterator {
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
				line_iterator() : m_it() {};
				line_iterator(iterator const& it) : m_it(it) {};

				// Opérateurs
				// - déférenciation
				reference operator *  () const { return *m_it; }
				pointer   operator -> () const { return m_it.operator ->(); }
				reference operator [] (Matrice::size_type v) const { return m_it[v]; }

				// - comparaison
				bool operator <  (line_iterator const& lit) const { return m_it <  lit.m_it; };
				bool operator <= (line_iterator const& lit) const { return m_it <= lit.m_it; };
				bool operator == (line_iterator const& lit) const { return m_it == lit.m_it; };
				bool operator != (line_iterator const& lit) const { return m_it != lit.m_it; };
				bool operator >= (line_iterator const& lit) const { return m_it >= lit.m_it; };
				bool operator >  (line_iterator const& lit) const { return m_it >  lit.m_it; };

				// - incrémentation & décrémentation
				line_iterator& operator ++ () { m_it += LIG; return *this; }
				line_iterator& operator -- () { m_it -= LIG; return *this; }

				line_iterator operator ++ (int) {
					line_iterator tmp(*this);
					++(*this);
					return tmp;
				}

				line_iterator operator -- (int) {
					line_iterator tmp(*this);
					--(*this);
					return tmp;
				}

				// - assignation
				line_iterator& operator += (Matrice::size_type v) { m_it += v * LIG; return *this; }
				line_iterator& operator -= (Matrice::size_type v) { m_it -= v * LIG; return *this; }

				// - arithmétique
				line_iterator operator + (Matrice::size_type v) const { line_iterator r(*this); r += v; return r; };
				line_iterator operator - (Matrice::size_type v) const { line_iterator r(*this); r -= v; return r; };

				friend line_iterator operator + (Matrice::size_type v, line_iterator const& it) {
					line_iterator r(it);
					r += v;
					return r;
				};

				difference_type operator - (line_iterator lit) const {
					return (m_it - lit.m_it) / LIG;
				};
		};

		class const_line_iterator {
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
				const_line_iterator() : m_it() {};
				const_line_iterator(const_iterator const& it) : m_it(it) {};
				const_line_iterator(line_iterator const& lit) : m_it(lit.m_it) {};

				// Opérateurs
				// - déférenciation
				reference operator *  () const { return *m_it; }
				pointer   operator -> () const { return m_it.operator ->(); }
				reference operator [] (Matrice::size_type v) const { return m_it[v]; }

				// - comparaison
				bool operator <  (const_line_iterator const& lit) const { return m_it <  lit.m_it; };
				bool operator <= (const_line_iterator const& lit) const { return m_it <= lit.m_it; };
				bool operator == (const_line_iterator const& lit) const { return m_it == lit.m_it; };
				bool operator != (const_line_iterator const& lit) const { return m_it != lit.m_it; };
				bool operator >= (const_line_iterator const& lit) const { return m_it >= lit.m_it; };
				bool operator >  (const_line_iterator const& lit) const { return m_it >  lit.m_it; };

				// - incrémentation & décrémentation
				const_line_iterator& operator ++ () { m_it += LIG; return *this; }
				const_line_iterator& operator -- () { m_it -= LIG; return *this; }

				const_line_iterator operator ++ (int) {
					const_line_iterator tmp(*this);
					++(*this);
					return tmp;
				}

				const_line_iterator operator -- (int) {
					const_line_iterator tmp(*this);
					--(*this);
					return tmp;
				}

				// - assignation
				const_line_iterator& operator += (Matrice::size_type v) { m_it += v * LIG; return *this; }
				const_line_iterator& operator -= (Matrice::size_type v) { m_it -= v * LIG; return *this; }

				// - arithmétique
				const_line_iterator operator + (Matrice::size_type v) const { const_line_iterator r(*this); r += v; return r; };
				const_line_iterator operator - (Matrice::size_type v) const { const_line_iterator r(*this); r -= v; return r; };

				friend const_line_iterator operator + (Matrice::size_type v, const_line_iterator const& it) {
					const_line_iterator r(it);
					r += v;
					return r;
				};

				difference_type operator - (const_line_iterator lit) const {
					return (m_it - lit.m_it) / LIG;
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
				reference operator [] (size_type const& c) {
					return *(m_deb + c);
				}

				const_reference operator [] (size_type const& c) const {
					return *(const_iterator(m_deb) + c);
				}

				// Méthodes
				size_type size() const {
					return N;
				}

				void fill(Int const& v) {
					for (size_type i = 0; i < size(); ++i) (*this)[i] = v;
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
		Matrice() : hash(LIG) {};
		Matrice(std::array<Int, LIG * COL> const& matrice) : hash(LIG), m_matrice(matrice) {};

		// Opérateurs
		// - accès élément
		reference operator [] (Coord const& c) { // (col, lig)
			return m_matrice[hash(c)];
		}

		const_reference operator [] (Coord const& c) const { // (col, lig)
			return m_matrice[hash(c)];
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

		auto column(size_type c) {
			return part<iterator,const_iterator,LIG>(begin() + (c * LIG));
		}

		auto column(size_type c) const {
			return const_part<const_iterator,LIG>(begin() + (c * LIG));
		}

		auto line(size_type l) {
			return part<line_iterator,const_line_iterator,COL>(begin() + l);
		}

		auto line(size_type l) const {
			return const_part<const_line_iterator,COL>(begin() + l);
		}

		// Itérateurs
		iterator begin() { return m_matrice.begin(); }
		iterator end()   { return m_matrice.end(); }

		const_iterator begin() const { return m_matrice.cbegin(); }
		const_iterator end()   const { return m_matrice.cend(); }
};

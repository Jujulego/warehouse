#pragma once

// Imporation
#include <iterator>
#include <stdexcept>
#include <vector>

#include "coord.hpp"

// Classe
template<class Int, class Alloc = std::allocator<Int>>
class Matrice {
	public:
		// Alias
		using allocator_type   = Alloc;
		using value_type       = typename Alloc::value_type;
		using reference        = typename Alloc::reference;
		using const_reference  = typename Alloc::const_reference;
		using pointer          = typename Alloc::pointer;
		using const_pointer    = typename Alloc::const_pointer;
		using iterator         = typename std::vector<Int,Alloc>::iterator;
		using const_iterator   = typename std::vector<Int,Alloc>::const_iterator;
		using size_type        = typename Alloc::size_type;
		using difference_type  = typename Alloc::difference_type;

		// Sous-classes
		class column_iterator {
			public:
				// Alias
				using value_type        = Matrice::value_type;
				using reference         = Matrice::reference;
				using difference_type   = Matrice::difference_type;
				using iterator_category = std::random_access_iterator_tag;

				// Constructeurs
				column_iterator() {};
				column_iterator(iterator const& it, size_type col) : m_it(it), COL(col) {};

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

			private:
				// Attributs
				iterator m_it;
				size_type COL;
		};

		class const_column_iterator {
			public:
				// Alias
				using value_type        = Matrice::value_type;
				using reference         = Matrice::const_reference;
				using difference_type   = Matrice::difference_type;
				using iterator_category = std::random_access_iterator_tag;

				// Constructeurs
				const_column_iterator() : m_it() {};
				const_column_iterator(column_iterator const& cit) : m_it(cit.m_it), COL(cit.COL) {};
				const_column_iterator(const_iterator const& it, size_type col) : m_it(it), COL(col) {};

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

			private:
				// Attributs
				const_iterator m_it;
				size_type COL;
		};

		template<class It, class CIt>
		class part {
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
				part() {}
				part(It const& deb, size_type n) : m_deb(deb), N(n) {};

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

			private:
				// Attributs
				It m_deb;
				size_type N;
		};

		template<class CIt>
		class const_part {
			public:
				// Alias
				using value_type       = Matrice::value_type;
				using const_reference  = Matrice::const_reference;
				using const_iterator   = CIt;
				using size_type        = Matrice::size_type;
				using difference_type  = Matrice::difference_type;

				// Constructeurs
				template<class It>
				const_part(part<It,CIt> const& p) : m_deb(p.m_deb), N(p.N) {};
				const_part(CIt const& deb, size_type n) : m_deb(deb), N(n) {};

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

			private:
				// Attributs
				CIt m_deb;
				size_type N;
		};

		// Constructeur
		Matrice(size_type lig)                : LIG(lig), COL(lig), hash(lig), m_matrice(lig * lig) {};
		Matrice(size_type lig, size_type col) : LIG(lig), COL(col), hash(lig), m_matrice(lig * col) {};

		Matrice(std::vector<std::vector<Int>> const& matrice)
			: LIG(matrice.size()), COL(matrice.front().size()),
			  hash(matrice.size()), m_matrice(matrice.size() * matrice.front().size()) {

			for (int l = 0; l < LIG; ++l) {
				for (int c = 0; c < COL; ++c) {
					m_matrice[hash(l, c)] = matrice[l][c];
				}
			}
		};

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
			if (nb_lig() != m.nb_lig() || nb_col() != m.nb_col()) throw std::domain_error("Cannot add 2 matrix of different size");

			for (int i = 0; i < size(); ++i) {
				m_matrice[i] += m.m_matrice[i];
			}

			return *this;
		}

		Matrice& operator -= (Matrice const& m) {
			if (nb_lig() != m.nb_lig() || nb_col() != m.nb_col()) throw std::domain_error("Cannot substract 2 matrix of different size");

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
            for (size_t i = 0; i < size(); ++i) m_matrice[i] = val;
		}

		auto colonne(size_type c) {
			return part<column_iterator,const_column_iterator>(column_iterator(begin() + c, COL), LIG);
		}

		auto colonne(size_type c) const {
			return const_part<const_column_iterator>(const_column_iterator(begin() + c, COL), LIG);
		}

		auto colonnes() {
			std::vector<part<column_iterator,const_column_iterator>> colonnes(COL);

			for (int c = 0; c < COL; ++c) {
				colonnes[c] = colonne(c);
			}

			return colonnes;
		}

		auto colonnes() const {
			std::vector<const_part<const_column_iterator>> colonnes(COL);

			for (int c = 0; c < COL; ++c) {
				colonnes[c] = colonne(c);
			}

			return colonnes;
		}

		auto ligne(size_type l) {
			return part<iterator,const_iterator>(begin() + (l * COL), COL);
		}

		auto ligne(size_type l) const {
			return const_part<const_iterator>(begin() + (l * COL), COL);
		}

		auto lignes() {
			std::vector<part<iterator,const_iterator>> lignes(LIG);

			for (int l = 0; l < LIG; ++l) {
				lignes[l] = ligne(l);
			}

			return lignes;
		}

		auto lignes() const {
			std::vector<const_part<const_iterator>> lignes(LIG);

			for (int l = 0; l < LIG; ++l) {
				lignes[l] = ligne(l);
			}

			return lignes;
		}

		// Itérateurs
		iterator begin() { return m_matrice.begin(); }
		iterator end()   { return m_matrice.end(); }

		const_iterator begin() const { return m_matrice.cbegin(); }
		const_iterator end()   const { return m_matrice.cend(); }

	private:
		// Attributs
		size_type LIG, COL;
		std::hash<Coord> hash;
		std::vector<Int,Alloc> m_matrice;
};

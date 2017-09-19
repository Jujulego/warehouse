#ifndef __UTILS_MANIP
#define __UTILS_MANIP

// Importations
#include <ostream>
#include <sstream>
#include <string>

// -------------- BaseManip ------------
class BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const = 0;
};

// Opérateur externe
std::ostream& operator << (std::ostream& stream, BaseManip const& manip);

// ------------- ClearManip ------------
class ClearManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ------------- CoordManip ------------
class CoordManip : public BaseManip {
	protected:
		// Attributs
		int m_x;
		int m_y;

	public:
		// Constructeur
		CoordManip(int x = 0, int y = 0);

		// Opérateurs
		CoordManip  operator +  () const;
		CoordManip  operator -  () const;
		CoordManip  operator +  (CoordManip const& cm) const;
		CoordManip  operator -  (CoordManip const& cm) const;
		CoordManip  operator *  (int const& k)         const;
		CoordManip  operator /  (int const& k)         const;
		CoordManip& operator += (CoordManip const& cm);
		CoordManip& operator -= (CoordManip const& cm);
		CoordManip& operator *= (int const& k);
		CoordManip& operator /= (int const& k);

		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// Opérateur externe
CoordManip operator * (int const& k, CoordManip const& cm);

// ------------- MouvManip -------------
class MouvManip : public CoordManip {
	public:
		// Constructeur
		MouvManip(int dx, int dy);

		// Opérateurs
		MouvManip  operator +  () const;
		MouvManip  operator -  () const;
		MouvManip  operator +  (MouvManip const& cm) const;
		MouvManip  operator -  (MouvManip const& cm) const;
		MouvManip  operator *  (int const& k)         const;
		MouvManip  operator /  (int const& k)         const;
		MouvManip& operator += (MouvManip const& cm);
		MouvManip& operator -= (MouvManip const& cm);
		MouvManip& operator *= (int const& k);
		MouvManip& operator /= (int const& k);

		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// Opérateur externe
MouvManip operator * (int const& k, MouvManip const& cm);

// ----------- EffLigneManip -----------
class EffLigneManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ------------ SauveManip -------------
class SauveManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ----------- RestoreManip ------------
class RestoreManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ------------- BuzzManip -------------
class BuzzManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// Instances
namespace manip {
	// Basiques
	static ClearManip clear;         // Efface l'écran

	// Coords
	static CoordManip origine(0, 0); // Le point en haut à gauche
	static CoordManip x(1, 0);
	static CoordManip y(0, 1);
	inline CoordManip coord(int x, int y) { return CoordManip(x, y); }

	// Basiques
	static EffLigneManip eff_ligne;  // Efface la ligne
	static SauveManip    sauve;      // Sauvegarde la position du curseur
	static RestoreManip  restore;    // Restore la dernière position sauvegardée
	static BuzzManip     buzz;       // Fait buzzer l'ordi

	// Mouv
	static MouvManip dx(1, 0);       // Décalage du curseur
	static MouvManip dy(0, 1);
};

#ifdef __gnu_linux__
// ------------- posstream -------------
template<class Stream>
class posstream {
	private:
		// Attributs
		int m_nb = 0;
		int m_lig = 0;
		Stream* m_flux;
		CoordManip m_coord;

	public:
		// Constructeur
		posstream(Stream* flux, CoordManip coord) : m_flux(flux), m_coord(coord)  {}
		posstream(Stream* flux, int x, int y) : posstream(flux, CoordManip(x, y)) {}

		// Opérateurs
		template<class T>
		posstream<Stream>& operator << (T const& obj) {
			// Convertion string
			std::ostringstream oss;
			oss << obj;

			// Affichage & decompte
			*m_flux << manip::sauve << m_coord + m_nb * manip::x + m_lig * manip::y << oss.str() << manip::restore;

			for (char c : oss.str()) {
				m_nb++;
				if (c == '\n') m_lig++;
			}

			return *this;
		}

		posstream<Stream>& operator << (Stream& (*manip)(Stream&)) {
			// Génération du string
			std::ostringstream oss;
			oss << manip;

			// Affichage
			*m_flux << manip::sauve << m_coord + m_nb * manip::x + m_lig * manip::y << oss.str() << manip::restore;

			for (char c : oss.str()) {
				m_nb++;
				if (c == '\n') {
					m_lig++;
					m_nb = 0;
				}
			}

			return *this;
		}

		posstream<Stream>& operator << (EffLigneManip const&) {
			*m_flux << manip::sauve << m_coord + m_lig * manip::y;

			for (int i = 0; i < m_nb; ++i) {
				*m_flux << ' ';
			}

			*m_flux << manip::restore;
			m_nb = 0;

			return *this;
		}

		// Méthodes
		void flush() {
			m_flux->flush();
		}
};
#endif

#endif // __UTILS_MANIP

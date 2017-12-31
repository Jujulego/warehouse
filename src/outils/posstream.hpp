#pragma once

// Importations
#include <ostream>
#include <sstream>
#include <string>

#include "coord.hpp"
#include "manip.hpp"
#include "style.hpp"

// Classe
template<class Stream>
class posstream {
	private:
		// Attributs
		int m_nb = 0;
		int m_lig = 0;
		Stream* m_flux;
		CoordManip m_coord;
		Style m_style = style::defaut;

	public:
		// Constructeur
		posstream(Stream* flux, Coord const& c) : m_flux(flux), m_coord(c)    {}
		posstream(Stream* flux, int x, int y)   : m_flux(flux), m_coord(x, y) {}

		// Opérateurs
		template<class T>
		posstream<Stream>& operator << (T const& obj) {
			// Convertion string
			std::ostringstream oss;
			oss << obj;

			// Affichage & decompte
			*m_flux << manip::sauve << m_coord + m_nb * manip::x + m_lig * manip::y;
			*m_flux << m_style << oss.str() << style::defaut;
			*m_flux << manip::restore;

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
			*m_flux << manip::sauve << m_coord + m_nb * manip::x + m_lig * manip::y;
			*m_flux << m_style << oss.str() << style::defaut;
			*m_flux << manip::restore;

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
			*m_flux << manip::sauve << m_coord + m_lig * manip::y << style::defaut;

			for (int i = 0; i < m_nb; ++i) {
				*m_flux << ' ';
			}

			*m_flux << manip::restore;
			m_nb = 0;

			return *this;
		}

		posstream<Stream>& operator << (CoordManip const& cm) {
			m_nb  = cm.x();
			m_lig = cm.y();

			return *this;
		}

		posstream<Stream>& operator << (MouvManip const& mm) {
			m_nb  += mm.x();
			m_lig += mm.y();

			return *this;
		}

		posstream<Stream>& operator << (Style const& s) {
			if (s.txt() != style::ACTUELLE) m_style.txt(s.txt());
			if (s.fnd() != style::ACTUELLE) m_style.fnd(s.fnd());
			m_style.effet(s.effet());

			return *this;
		}

		// Méthodes
		CoordManip& coord() {
			return m_coord;
		}

		void flush() {
			m_flux->flush();
		}

		void style(Style const& style) {
			m_style = style;
		}
};

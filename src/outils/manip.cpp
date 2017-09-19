// Importations
#include <ostream>
#include <string>

#include "manip.hpp"

#ifndef __gnu_linux__
# include <windows.h>
#endif

// Opérateur externe
std::ostream& operator << (std::ostream& stream, BaseManip const& manip) {
	return manip.appliquer(stream);
}

// ------------- ClearManip ------------
// Méthodes
std::ostream& ClearManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\x1b[2J";
#else
	system("cls");
	return stream;
#endif
}

// ------------- CoordManip ------------
// Constructeur
CoordManip::CoordManip(int x, int y)
	: m_x(x), m_y(y) {
}

// Opérateurs
CoordManip  CoordManip::operator +  ()                     const { return CoordManip(+m_x, +m_y); }
CoordManip  CoordManip::operator -  ()                     const { return CoordManip(-m_x, -m_y); }
CoordManip  CoordManip::operator +  (CoordManip const& cm) const { return CoordManip(m_x + cm.m_x, m_y + cm.m_y); }
CoordManip  CoordManip::operator -  (CoordManip const& cm) const { return CoordManip(m_x - cm.m_x, m_y - cm.m_y); }
CoordManip  CoordManip::operator *  (int const& k)         const { return CoordManip(k*m_x, k*m_y); }
CoordManip  CoordManip::operator /  (int const& k)         const { return CoordManip(m_x/k, m_y/k); }
CoordManip& CoordManip::operator += (CoordManip const& cm)       { m_x += cm.m_x; m_y += cm.m_y; return *this; }
CoordManip& CoordManip::operator -= (CoordManip const& cm)       { m_x -= cm.m_x; m_y -= cm.m_y; return *this; }
CoordManip& CoordManip::operator *= (int const& k)               { m_x *= k;      m_y *= k;      return *this; }
CoordManip& CoordManip::operator /= (int const& k)               { m_x /= k;      m_y /= k;      return *this; }

// Opérateurs externes
CoordManip operator * (int const& k, CoordManip const& cm)       { return cm * k; }

// Méthodes
std::ostream& CoordManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\x1b[" << m_y+1 << ";" << m_x+1 << "H";
#else
	COORD coord;
    coord.X = m_x;
    coord.Y = m_y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	return stream;
#endif
}

// ------------- MouvManip -------------
// Constructeur
MouvManip::MouvManip(int dx, int dy) : CoordManip(dx, dy) {
}

// Opérateurs
MouvManip  MouvManip::operator +  ()                    const { return MouvManip(+m_x, +m_y); }
MouvManip  MouvManip::operator -  ()                    const { return MouvManip(-m_x, -m_y); }
MouvManip  MouvManip::operator +  (MouvManip const& cm) const { return MouvManip(m_x + cm.m_x, m_y + cm.m_y); }
MouvManip  MouvManip::operator -  (MouvManip const& cm) const { return MouvManip(m_x - cm.m_x, m_y - cm.m_y); }
MouvManip  MouvManip::operator *  (int const& k)        const { return MouvManip(k*m_x, k*m_y); }
MouvManip  MouvManip::operator /  (int const& k)        const { return MouvManip(m_x/k, m_y/k); }
MouvManip& MouvManip::operator += (MouvManip const& cm)       { m_x += cm.m_x; m_y += cm.m_y; return *this; }
MouvManip& MouvManip::operator -= (MouvManip const& cm)       { m_x -= cm.m_x; m_y -= cm.m_y; return *this; }
MouvManip& MouvManip::operator *= (int const& k)              { m_x *= k;      m_y *= k;      return *this; }
MouvManip& MouvManip::operator /= (int const& k)              { m_x /= k;      m_y /= k;      return *this; }

// Opérateurs externes
MouvManip operator * (int const& k, MouvManip const& cm)      { return cm * k; }

// Méthodes
std::ostream& MouvManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	// Verticalement
	if (m_y < 0) stream << "\x1b[" << (-m_y) << "A";
	if (m_y > 0) stream << "\x1b[" <<   m_y  << "B";

	if (m_x < 0) stream << "\x1b[" << (-m_x) << "D";
	if (m_x > 0) stream << "\x1b[" <<   m_x  << "C";
#else
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(h, &csbi);

	COORD coord = csbi.dwCursorPosition;
    coord.X += m_x;
    coord.Y += m_y;

	SetConsoleCursorPosition(h, coord);
#endif
	return stream;
}

// ----------- EffLigneManip -----------
// Méthodes
std::ostream& EffLigneManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\x1b[2K";
#else
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(h, &csbi);

	COORD coord = csbi.dwCursorPosition;
    coord.X = 0;

	SetConsoleCursorPosition(h, coord);

	for (int i = 0; i < csbi.srWindow.Right; ++i)
		stream << ' ';

	SetConsoleCursorPosition(h, csbi.dwCursorPosition);
	return stream;
#endif
}

#ifndef __gnu_linux__
static COORD curseur_coord;
#endif

// ------------ SauveManip -------------
// Méthodes
std::ostream& SauveManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\x1b[s";
#else
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	curseur_coord = csbi.dwCursorPosition;

	return stream;
#endif
}

// ----------- RestoreManip ------------
// Méthodes
std::ostream& RestoreManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\x1b[u";
#else
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), curseur_coord);

	return stream;
#endif
}

// ------------- BuzzManip -------------
// Méthodes
std::ostream& BuzzManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\a";
#else
    Beep(150, 300);
	return stream;
#endif
}

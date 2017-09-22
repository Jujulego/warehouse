// Importations
#include <ostream>
#include <string>

#include "coord.hpp"
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
CoordManip::CoordManip(Coord const& c) : Coord(c) {}
CoordManip::CoordManip(int x, int y)   : Coord(x, y) {}

// Opérateurs
CoordManip  CoordManip::operator +  ()               const { return CoordManip(+x(),       +y()); }
CoordManip  CoordManip::operator -  ()               const { return CoordManip(-x(),       -y()); }
CoordManip  CoordManip::operator +  (Coord const& c) const { return CoordManip(x() + c.x(), y() + c.y()); }
CoordManip  CoordManip::operator -  (Coord const& c) const { return CoordManip(x() - c.x(), y() - c.y()); }
CoordManip  CoordManip::operator *  (int k)          const { return CoordManip(x() * k,     y() * k); }
CoordManip  CoordManip::operator /  (int k)          const { return CoordManip(x() / k,     y() / k); }
CoordManip& CoordManip::operator += (Coord const& c)       { x() += c.x(); y() += c.y(); return *this; }
CoordManip& CoordManip::operator -= (Coord const& c)       { x() -= c.x(); y() -= c.y(); return *this; }
CoordManip& CoordManip::operator *= (int k)                { x() *= k;     y() *= k;     return *this; }
CoordManip& CoordManip::operator /= (int k)                { x() /= k;     y() /= k;     return *this; }

// Opérateurs externes
CoordManip operator * (int const& k, CoordManip const& cm)       { return cm * k; }

// Méthodes
std::ostream& CoordManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	return stream << "\x1b[" << y()+1 << ";" << x()+1 << "H";
#else
	COORD coord;
    coord.X = x();
    coord.Y = y();

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	return stream;
#endif
}

// ------------- MouvManip -------------
// Constructeur
MouvManip::MouvManip(Coord const& c) : CoordManip(c) {}
MouvManip::MouvManip(int dx, int dy) : CoordManip(dx, dy) {}

// Opérateurs
MouvManip  MouvManip::operator +  ()               const { return MouvManip(+x(),       +y()); }
MouvManip  MouvManip::operator -  ()               const { return MouvManip(-x(),       -y()); }
MouvManip  MouvManip::operator +  (Coord const& c) const { return MouvManip(x() + c.x(), y() + c.y()); }
MouvManip  MouvManip::operator -  (Coord const& c) const { return MouvManip(x() - c.x(), y() - c.y()); }
MouvManip  MouvManip::operator *  (int k)          const { return MouvManip(x() * k,     y() * k); }
MouvManip  MouvManip::operator /  (int k)          const { return MouvManip(x() / k,     y() / k); }
MouvManip& MouvManip::operator += (Coord const& c)       { x() += c.x(); y() += c.y(); return *this; }
MouvManip& MouvManip::operator -= (Coord const& c)       { x() -= c.x(); y() -= c.y(); return *this; }
MouvManip& MouvManip::operator *= (int k)                { x() *= k;     y() *= k;     return *this; }
MouvManip& MouvManip::operator /= (int k)                { x() /= k;     y() /= k;     return *this; }

// Opérateurs externes
MouvManip operator * (int const& k, MouvManip const& mm)      { return mm * k; }

// Méthodes
std::ostream& MouvManip::appliquer(std::ostream& stream) const {
#ifdef __gnu_linux__
	// Verticalement
	if (y() < 0) stream << "\x1b[" << (-y()) << "A";
	if (y() > 0) stream << "\x1b[" <<   y()  << "B";

	if (x() < 0) stream << "\x1b[" << (-x()) << "D";
	if (x() > 0) stream << "\x1b[" <<   x()  << "C";
#else
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(h, &csbi);

	COORD coord = csbi.dwCursorPosition;
    coord.X += x();
    coord.Y += y();

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

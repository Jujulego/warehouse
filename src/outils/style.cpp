// Importations
#include <mutex>
#include <ostream>

#include "style.hpp"

// Abstraction Windows
#ifndef __gnu_linux__
# include <atomic>
# include <conio.h>
# include <map>
# include <windows.h>
#endif

// Namespaces
using namespace style;

// Mutex
static std::mutex mtx_style;

// Méthodes
std::ostream& Style::appliquer(std::ostream& stream) const {
	// Verrouillage
	std::unique_lock<std::mutex> lck(mtx_style);
	
	// Sauvegarde des drapeaux
	auto flags = stream.flags();
	
#ifdef __gnu_linux__
	// Application du style ! (version Linux)
	if (m_effet == NORMAL) stream << "\x1b[m";
	
	stream << std::noshowbase << std::hex << "\x1b[";
	
	bool ptvirg = false;
	if (m_effet != NORMAL) {
		stream << m_effet;
		ptvirg = true;
	}
	
	if (m_fnd != ACTUELLE) {
		if (ptvirg) stream << ';';
		stream << (m_fnd | FOND);
		ptvirg = true;
	}
	
	if (m_txt != ACTUELLE) {
		if (ptvirg) stream << ';';
		stream << (m_txt | TEXTE);
	}
	
	stream << "m";
	stream.flush();
#else
	// Application du style ! (version Windows)
	static std::atomic<int> actuelle_txt(0b1111);
	static std::atomic<int> actuelle_fnd(0b0000);
	static std::map<Effet,bool> effet_actuel = {
		{NORMAL,     true},
		{GRAS,       false},
		{SOMBRE,     false},
		{ITALIQUE,   false},
		{SOULIGNE,   false},
		{CLIGNOTANT, false},
		{INVERSE,    false},
		{INVISIBLE,  false},
		{BARRE,      false},
    };

	static const std::map<Couleur,int> CWIN = {
		{ACTUELLE,     -1},
		{DEFAUT_TEXTE, 0b1111},
		{DEFAUT_FOND,  0b0000},

		// Format : IRVB
		{NOIR,    0b1000},
		{ROUGE,   0b1100},
		{VERT,    0b1010},
		{JAUNE,   0b1110},
		{BLEU,    0b1001},
		{VIOLET,  0b1101},
		{CYAN,    0b1011},
		{BLANC,   0b1111},
	};

	// Récupération version windows
	int txt = CWIN.at(m_txt), fnd = CWIN.at(m_fnd);

	// Cas de la couleur "Actuelle" => pas de chg apparent
	if (txt == -1) txt = actuelle_txt; else actuelle_txt = txt;
	if (fnd == -1) fnd = actuelle_fnd; else actuelle_fnd = fnd;

	// Gestion des effets
	if (m_effet < 0x20) {
        effet_actuel[m_effet] = true;
	} else {
        effet_actuel[Effet(m_effet-0x20)] = false;
	}
	if (m_effet == NORMAL) { // Reset complet
        for (auto it = effet_actuel.begin(); it != effet_actuel.end(); it++)
            it->second = false;
	}

	// Inverse !
	int effet = 0;
	if (effet_actuel[INVERSE]) {
        txt = actuelle_fnd;
        fnd = actuelle_txt;
	} else if (effet_actuel[SOMBRE]) {
        txt &= 0b0111;
        txt &= 0b0111;
	}

	// Application !
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(H, (fnd << 4) | txt | effet);
#endif

	// Retour des drapeaux
	stream.flags(flags);
	return stream;
}

// Accesseurs
style::Couleur Style::txt() const {
	return m_txt;
}

style::Couleur Style::fnd() const {
	return m_fnd;
}

style::Effet  Style::effet() const {
	return m_effet;
}

void Style::txt(style::Couleur const& txt) {
	m_txt = txt;
}

void Style::fnd(style::Couleur const& fnd) {
	m_fnd = fnd;
}

void Style::effet(style::Effet const& effet) {
	m_effet = effet;
}

// Opérateurs externes
std::ostream& operator << (std::ostream& stream, Style const& style) {
	return style.appliquer(stream);
}

// Importations
#include <iostream>
#include <streambuf>

#include "console.hpp"

// Couche d'abstraction win <> lin
namespace c {
extern "C" {
#ifndef __gnu_linux__
# include <conio.h>
# include <windows.h>
#else
# include <termios.h>
# include <fcntl.h>
# include <unistd.h>

int getch() {
	// Déclarations
	struct termios oldattr, newattr;
	int ch;

	// Sauvegarde de la config
	tcgetattr(STDIN_FILENO, &oldattr);

	// Modification du terminal
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHOPRT | ECHOKE);
	newattr.c_iflag &= ~ICRNL;
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

	// Récupération du char
	ch = getchar();

	// Rétablissement du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

	return ch;
}

int kbhit() {
	// Déclarations
	struct termios oldt, newt;
	int ch, oldf;

	// Sauvegarde de la config
	tcgetattr(STDIN_FILENO, &oldt);

	// Modification du terminal
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHOPRT | ECHOKE);
	newt.c_iflag &= ~ICRNL;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	// Préparation de stdin (rendu non bloquant)
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	// Récupération d'un charactère (ou EOF)
	ch = getchar();

	// Rétablissement
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	// Test !
	if (ch != EOF) {
		// Renvoi du charactère
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}
#endif
}
}

using namespace c;

// Méthodes
void Console::gotoLigCol(int lig, int col) { // déplacement du curseur
#ifndef __gnu_linux__
	c::COORD mycoord;
	mycoord.X = col;
	mycoord.Y = lig;

	c::SetConsoleCursorPosition(c::GetStdHandle(STD_OUTPUT_HANDLE), mycoord);
#else
	std::cout << "\x1b[" << lig+1 << ";" << col+1 << "H";
#endif
}

void Console::clear() {
#ifndef __gnu_linux__
	system("cls");
#else
	std::cout << "\x1b[2J";
#endif
}

std::string Console::getch() { // récupère le caractère du clavier
	// Nettoyage
	std::cin.clear();
	
#ifdef __gnu_linux__
    // Déclarations
	std::string str(1, c::getch());
	
	// Récupération des charactères
	while (c::kbhit()) {
		char ch = c::getch();
		str.append(1, ch);
		
		if (str[0] == '\x1b' && (ch == '~' || (ch >= 'A' && ch <= 'Z'))) break;
	}
#else
	// Déclarations
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	DWORD nb = 0, flags = 0;
	INPUT_RECORD rec;
	std::string str;
	WCHAR c;

	// Récupération
    GetConsoleMode(h, &flags);
    SetConsoleMode(h, flags & ~(ENABLE_LINE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT));

    while ((nb == 0) || !rec.Event.KeyEvent.bKeyDown) {
        FlushConsoleInputBuffer(h);
        ReadConsoleInput(h, &rec, 1, &nb);
    }

    SetConsoleMode(h, flags);

	// Construction du résultat
	if (nb > 0) {
        c = rec.Event.KeyEvent.uChar.UnicodeChar;
        std::cout << c << std::endl;

        for (int i = sizeof(WCHAR); i != 0; i--) {
            str.append(1, c%255);
            c >>= 8;
        }
	}
#endif // __gnu_linux__
	
	return str;
}

bool Console::kbhit() { // renvoie 0 ou 1
	std::cin.clear();
	return c::kbhit() != 0;
}

void Console::reset() {
#ifdef __gnu_linux__
    // Rétablissement du terminal
    struct termios attrs;
    
    tcgetattr(STDIN_FILENO, &attrs);
    
    attrs.c_lflag |= (ICANON | ECHO | ECHOE | ECHOK | ECHOPRT | ECHOKE);
    attrs.c_iflag |= ICRNL;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &attrs);
#endif
}

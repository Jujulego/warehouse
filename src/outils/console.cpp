// Importations
#include <iostream>
#include <mutex>

#include "console.hpp"
#include "manip.hpp"

namespace c {

// Couche d'abstraction :
#ifndef __gnu_linux__
# include <conio.h>
# include <windows.h>
#else
# include <fcntl.h>
# include <unistd.h>
# include <termios.h>

int getch(void) {
	struct termios oldattr, newattr;
	int ch, oldf;
	
	// Sauvegarde de la config
	tcgetattr(STDIN_FILENO, &oldattr);
	
	// Modification du terminal
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	
	// Vidage du buffer
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	
	while (getchar() != EOF) {}
	
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	
	// Récupération du char
	ch = getchar();
	
	if (ch == 27) { // char de controle
		ch <<= 16;
		ch += getchar() << 8;
		ch += getchar();
	}
	
	// Rétablissement du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	
	return ch;
}

int kbhit() {
	struct termios oldt, newt;
	int ch, oldf;
	
	// Sauvegarde de la config
	tcgetattr(STDIN_FILENO, &oldt);
	
	// Modification du terminal
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	
	// Préparation de stdin (rendu non bloquant)
	fflush(stdin);
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

} // c

using namespace c;

// Méthodes
void console::gotoLigCol(int lig, int col) { // déplacement du curseur
	std::cout << manip::coord(lig, col);
}

void console::clear() {
	std::cout << manip::clear;
}

int console::getch(bool bloc) { // récupère le caractère du clavier
	std::cin.clear();
	if (!bloc && !c::kbhit()) return 0;
	int c = c::getch();
	
#ifndef __gnu_linux__
	if (c == 224) {
		c <<= 8;
		c += c::getch();
	}
#endif // __gnu_linux__
	
	return c;
}

int console::kbhit() { // renvoi 0 ou 1
	std::cin.clear();
	return c::kbhit();
}

// Verrou d'affichage !
static std::mutex VERROU;

std::unique_lock<std::mutex> console::lock() {
	return std::unique_lock<std::mutex>(VERROU);
}

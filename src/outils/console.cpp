// Importations
#include "console.hpp"

namespace c {
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
}

using namespace c;

// Méthodes
void Console::gotoLigCol(int lig, int col) { // déplacement du curseur
#ifndef __gnu_linux__
    COORD mycoord;
    mycoord.X = col;
    mycoord.Y = lig;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), mycoord);
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

int Console::getch() { // récupère le caractère du clavier
    std::cin.clear();
    int c = c::getch();

#ifndef __gnu_linux__
    if (c == 224) {
        c <<= 8;
        c += c::getch();
    }
#endif // __gnu_linux__

    return c;
}

int Console::kbhit() { // renvoi 0 ou 1
    std::cin.clear();
    return c::kbhit();
}

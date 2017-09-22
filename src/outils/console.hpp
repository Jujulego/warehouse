#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

// Dépendances
#include <iostream>

// Classe
/// Gestion de la console
class Console {
    public:
        // Méthodes publiques
        void gotoLigCol(int lig, int col);
        void clear();
        int  kbhit();
        int  getch();
};

#endif // CONSOLE_H_INCLUDED

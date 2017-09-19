#ifndef __CONSOLE
#define __CONSOLE

// Importations
#include <string>

// Classe
class Console {
    public:
        // Méthodes publiques
        void gotoLigCol(int lig, int col);
        void clear();
        
        // Clavier
        bool kbhit();        /// Renvoie vrai si on appuye sur une touche
        std::string getch(); /// Récupere la touche du clavier
        static void reset();
};

#endif // __CONSOLE

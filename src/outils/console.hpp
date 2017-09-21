#pragma once

// Importations
#include <string>

// Classe
class Console {
	public:
		// Méthodes
		void gotoLigCol(int lig, int col);
		void clear();
		
		// Clavier
		bool kbhit();        /// Renvoie vrai si on appuye sur une touche
		std::string getch(); /// Récupere la touche du clavier
		static void reset();
};

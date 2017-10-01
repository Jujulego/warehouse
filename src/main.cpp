// Importations
#include <functional>

#include "menu.hpp"
#include "niveau.hpp"

// Main !
int main() {
	Menu menu;
	
	Niveau niv("carte.txt");
	menu.ajouter("Jouer", std::bind(&Niveau::jouer, niv));
	
	menu.afficher();
	
	return 0;
}

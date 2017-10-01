// Importations
#include <functional>
#include <iostream>

#include "menu.hpp"
#include "niveau.hpp"

// Main !
int main() {
	Menu menu;
	
	Niveau niv("carte.txt");
	std::cout << "cool !" << std::endl;
	menu.ajouter("Jouer", std::bind(&Niveau::jouer, niv));
	std::cout << "cool !" << std::endl;
	
	menu.afficher(10, 5);
	
	return 0;
}

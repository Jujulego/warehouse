// Importations
#include <iostream>

#include "outils/manip.hpp"
#include "outils/style.hpp"

// Main !
int main() {
	std::cout << manip::clear << manip::buzz;
	std::cout << manip::coord(15, 10) << style::souligne <<
	             "Hello world ! ;)" << style::nonsouligne << std::endl;

	std::cout << manip::dx * 16 << style::bleu << style::inverse << "Pas mal hein ?" << style::noninverse << std::endl;

	return 0;
}

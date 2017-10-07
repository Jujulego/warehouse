#pragma once

// Dépendances
#include <iostream>

// Classe
namespace console {

// Prototypes
void gotoLigCol(int lig, int col);
void clear();
int kbhit();
int getch(bool bloc = true);

} // console

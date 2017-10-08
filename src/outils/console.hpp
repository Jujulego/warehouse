#pragma once

// Dépendances
#include <mutex>

// Classe
namespace console {

// Prototypes
void gotoLigCol(int lig, int col);
void clear();
int kbhit();
int getch(bool bloc = true);

std::unique_lock<std::mutex> lock();

} // console

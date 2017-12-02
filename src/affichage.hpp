#pragma once

// Importations
#include <memory>

#include "outils/coord.hpp"
#include "moteur/carte.hpp"
#include "moteur/obstacle.hpp"
#include "ia/solveur2.hpp"

// Macros
#ifdef __gnu_linux__
# define MUR    "\xe2\x97\x8b "
# define PERS   "\xe2\x98\xba"
# define BOITE  "\xe2\x9a"
# define EMPL   "[]"
# define SORTIE style::fvert << "  " << style::defaut
#else
# define MUR    "\xc4\xc4"
# define PERS   ";)"
# define BOITE  "#"
# define EMPL   "[]"
# define SORTIE style::fvert << "  " << style::defaut
#endif

// Prototypes
std::string select_mur(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Obstacle> const& mur);

void afficher_entete(int x, int y);
void afficher_entete(Coord const& c);

void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, int x, int y,   ia::Solveur2 const* solv2 = nullptr);
void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, Coord const& c, ia::Solveur2 const* solv2 = nullptr);

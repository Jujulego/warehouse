#pragma once

// Importations
#include <memory>

#include "moteur/carte.hpp"
#include "outils/coord.hpp"

// Prototype
void afficher_entete(int x, int y);
void afficher_entete(Coord const& c);

void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, int x, int y);
void afficher_carte(std::shared_ptr<moteur::Carte> const& carte, Coord const& c);

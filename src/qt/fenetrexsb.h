#ifndef FENETREXSB_H
#define FENETREXSB_H

#pragma once

// Importations
#include <string>
#include <map>

#include "menu.hpp"
#include "niveau.hpp"

class FenetreXSB
{
    public:

    // Constructeur
    Fenetrexsb();
    FichierXSB(std::string nom);

    // Méthodes
    int nb_niv() const;
    Menu as_menu() const;

    std::vector<Niveau> const& niveaux() const;

    private:
    // Méthodes
    void charger(std::string const& fichier);

    // Attributs
    std::vector<Niveau> m_niveaux;
};

#endif // FENETREXSB_H




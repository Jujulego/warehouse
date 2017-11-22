#pragma once

// Importations
#include <list>
#include <memory>

#include "moteur/carte.hpp"
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"

#include "ia/solveur3.hpp"

// Classe
class DevScreen {
	public:
		// Constructeur
		DevScreen(std::shared_ptr<moteur::Carte> carte);

		// Destructeur
		~DevScreen();

		// Méthodes
		void afficher();

	private:
		// Méthodes
		void afficher_carte() const;
		void afficher_status() const;

		void pop_poussables();
		void reset_poussables();

		// Attributs
		std::hash<Coord> hash;
		bool m_deplacables    = true;
		bool m_directions     = false;
		bool m_zone_interdite = false;
		bool m_zone_access    = true;
		bool m_portes         = false;
		bool m_poussees       = false;
		bool m_priorites      = false;
		bool m_tunnels        = false;
		bool m_stone_reachable = false;
		bool m_zones_empls    = false;

		ia::Solveur3* m_solv3;
		std::shared_ptr<moteur::Carte> m_carte;
		std::shared_ptr<moteur::Personnage> m_pers;
		std::list<std::shared_ptr<moteur::Poussable>> m_poussables;
};

// Importations
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "outils.hpp"
#include "outils/console.hpp"
#include "outils/coord.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"
#include "outils/style.hpp"

#include "moteur/carte.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/poussable.hpp"
#include "moteur/sortie.hpp"

#include "ia/solveur3.hpp"

#include "affichage.hpp"
#include "devscreen.hpp"

// Constructeur
DevScreen::DevScreen(std::shared_ptr<moteur::Carte> carte)
	: hash(carte->taille_y()), m_carte(std::make_shared<moteur::Carte>(*carte)) {

	m_pers  = m_carte->personnage();
	m_solv3 = new ia::Solveur3(m_carte, m_pers);
}

// Destructeur
DevScreen::~DevScreen() {
	delete m_solv3;
}

// Méthodes
void DevScreen::afficher() {
	// Entete
	std::cout << manip::clear;
	afficher_entete(ORIGINE);

	// Getch !
	bool continuer = true;
	while (continuer) {
		// Affichage !
		afficher_carte();
		afficher_status();

		// Interaction !
		Coord dir = ORIGINE;

		switch (console::getch()) {
		case FL_HAUT:
			dir = HAUT;
			break;

		case FL_BAS:
			dir = BAS;
			break;

		case FL_GAUCHE:
			dir = GAUCHE;
			break;

		case FL_DROITE:
			dir = DROITE;
			break;

		case 'a':
			m_deplacables = !m_deplacables;
			m_directions = false;
			break;

		case 'f':
			m_directions = !m_directions;
			m_deplacables = false;
			break;

		case 'p':
			if (m_poussables.size() == 0)
				pop_poussables();
			else
				reset_poussables();

			break;

		case 'q':
			continuer = false;
			break;
		}

		// Déplacement
		if (m_deplacables && dir != ORIGINE) {
			int p;
			m_pers->deplacer(dir, p);
		}
	}
}

void DevScreen::pop_poussables() {
	// Pops !
	for (auto po : m_carte->liste<moteur::Poussable>()) {
		(*m_carte)[po->coord()]->pop();
		m_poussables.push_back(po);
	}
}

void DevScreen::reset_poussables() {
	// Nettoyage
	for (auto po : m_carte->liste<moteur::Poussable>()) {
		(*m_carte)[po->coord()]->pop();
	}

	// Remise en place
	bool p = false;
	for (auto po : m_poussables) {
		if (m_pers->coord() == po->coord()) {
			p = true;
		} else {
			m_carte->set(po->coord(), po);
		}
	}

	if (!p) m_poussables.clear();
}

void DevScreen::afficher_status() const {
	// Initialisation
	static Coord ref(std::max(5 + m_carte->taille_x() * 2 + 4, 30), 11);
	static std::map<char,std::tuple<std::function<bool(DevScreen const&)>,std::string,std::string>> options = {
		{'A', {[] (DevScreen const& ds) { return ds.m_deplacables; },            "Cacher les déplacables  ", "Afficher les déplacables"}},
		{'F', {[] (DevScreen const& ds) { return ds.m_directions;  },            "Cacher les flèches  ",     "Afficher les flèches"}},
		{'P', {[] (DevScreen const& ds) { return ds.m_poussables.size() == 0; }, "Enlever les poussables ",  "Remettre les poussables"}},
	};

	// Affichage
	posstream<std::ostream> stream(&std::cout, ref);
	stream << "Commandes : " << 5 + m_carte->taille_x() * 2 + 4 << std::endl;
	for (auto opt : options) {
		stream << "- " << style::gras << opt.first << style::nongras << " : ";

		if (std::get<0>(opt.second)(*this)) {
			stream << std::get<1>(opt.second);
		} else {
			stream << std::get<2>(opt.second);
		}

		stream << std::endl;
	}

	stream << "- " << style::gras << "Q" << style::nongras << " : Retour au jeu !";
}

void DevScreen::afficher_carte() const {
	// Statiques !
#ifndef __gnu_linux__
	static const std::array<std::string,16> fleches = {
		"  ", " >", " <", " x",
		"v ", "v>", "v<", "vx",
		"^ ", "^>", "^<", "^x",
		"x ", "x>", "x<", "xx"
	};
#else
	static const std::array<std::string,16> fleches = {
		"  ", " →", " ←", " ↔",
		"↓ ", "↓→", "↓←", "↓↔",
		"↑ ", "↑→", "↑←", "↑↔",
		"↕ ", "↕→", "↕←", "↕↔"
	};
#endif

	// Initialisation
	auto ref = manip::coord(5, 9);
	
	// Affichage des objets
	for (auto obj : *m_carte) {
		std::cout << ref + obj->coord() + manip::x * obj->coord().x();
		auto dobj = obj->get();
		Style st;
		
		if (dobj && m_deplacables) {
			auto pobj = std::dynamic_pointer_cast<moteur::Poussable>(dobj);
			
			if (pobj) {
				if (m_carte->get<moteur::Emplacement>(pobj->coord())) {
					st.txt(style::VERT);
				} else {
					st.txt(style::JAUNE);
				}
				
				#ifdef __gnu_linux__
				if (pobj->poids() <= 6) {
					std::string s = BOITE;
					s.append(1, '\x7f' + pobj->poids());

					std::cout << st << s << ' ' << style::defaut;
				} else {
					std::cout << st << "#" << pobj->poids() << style::defaut;
				}
				#else
				std::cout << st << BOITE << pobj->poids() << style::defaut;
				#endif
			} else {
				st.txt(style::VERT);
				std::cout << st << PERS << style::defaut;
			}
		} else {
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << st << select_mur(m_carte, std::dynamic_pointer_cast<moteur::Obstacle>(obj));
			} else if (std::dynamic_pointer_cast<moteur::Sortie>(obj)) {
				std::cout << st << SORTIE;
			} else if (m_directions) {
				std::cout << st << fleches[m_solv3->infos_cases(m_carte)[hash(obj->coord())].directions];
			} else if (std::dynamic_pointer_cast<moteur::Emplacement>(obj)) {
				st.txt(style::DEFAUT_TEXTE);
				std::cout << st << EMPL;
			} else {
				std::cout << st << "  ";
			}
		}
	}
	
	// Reset style
	std::cout << style::defaut;
}

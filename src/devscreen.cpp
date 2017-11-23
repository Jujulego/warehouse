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
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
			break;

		case 'c':
			m_poussees = !m_poussees;
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
			m_zone_access = false;
			m_deplacables = true;
			break;

		case 'd':
			m_portes = !m_portes;
			m_poussees    = false;
			m_deplacables = false;
			m_directions  = false;
			m_zone_access = false;
			break;

		case 'e':
			m_priorites = !m_priorites;
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
			m_zone_access = false;
			m_deplacables = false;
			break;

		case 'f':
			m_directions = !m_directions;
			m_deplacables = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
			m_zone_access = false;
			break;

		case 'g':
			m_zones_empls = !m_zones_empls;
			m_poussees    = false;
			m_deplacables = false;
			m_directions  = false;
			m_zone_access = false;
			break;

		case 'h':
			m_intersections = !m_intersections;
			m_poussees    = false;
			m_deplacables = false;
			m_directions  = false;
			m_zone_access = false;
			break;

		case 'i':
			m_zone_interdite = !m_zone_interdite;
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

		case 's':
			m_stone_reachable = !m_stone_reachable;
			break;

		case 't':
			m_tunnels = !m_tunnels;
			m_poussees    = false;
			m_deplacables = false;
			m_directions  = false;
			m_zone_access = false;
			break;

		case 'z':
			m_zone_access = !m_zone_access;
			m_deplacables = true;
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
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
	static std::map<char,std::tuple<std::function<bool(DevScreen const&)>,std::string,std::string>> options = {
		{'A', {[] (DevScreen const& ds) { return ds.m_deplacables; },            "Cacher les déplacables  ",    "Afficher les déplacables"}},
		{'C', {[] (DevScreen const& ds) { return ds.m_poussees; },               "Cacher les poussees  ",       "Afficher les poussees"}},
		{'D', {[] (DevScreen const& ds) { return ds.m_portes; },                 "Cacher les portes  ",         "Afficher les portes"}},
		{'E', {[] (DevScreen const& ds) { return ds.m_priorites;  },             "Cacher les priorités  ",      "Afficher les priorités"}},
		{'F', {[] (DevScreen const& ds) { return ds.m_directions;  },            "Cacher les flèches  ",        "Afficher les flèches"}},
		{'G', {[] (DevScreen const& ds) { return ds.m_zones_empls;  },           "Cacher les zones empls  ",    "Afficher les zones empls"}},
		{'H', {[] (DevScreen const& ds) { return ds.m_intersections;  },         "Cacher les intersections  ",  "Afficher les intersections"}},
		{'I', {[] (DevScreen const& ds) { return ds.m_zone_interdite;  },        "Cacher la zone interdite  ",  "Afficher la zone interdite"}},
		{'P', {[] (DevScreen const& ds) { return ds.m_poussables.size() == 0; }, "Enlever les poussables ",     "Remettre les poussables"}},
		{'S', {[] (DevScreen const& ds) { return ds.m_stone_reachable;  },       "Cacher la zone SR  ",         "Afficher la zone SR"}},
		{'T', {[] (DevScreen const& ds) { return ds.m_tunnels;  },               "Cacher les tunnels  ",        "Afficher les tunnels"}},
		{'Z', {[] (DevScreen const& ds) { return ds.m_zone_access;  },           "Cacher la zone accessible  ", "Afficher la zone accessible"}},
	};

	// Affichage
	Coord ref(std::max(5 + m_carte->taille_x() * 2 + 4, 30), 9);
	posstream<std::ostream> stream(&std::cout, ref);
	stream << "Commandes : " << std::endl;
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
		"  ", " →", "← ", "←→",
		"↓ ", "↓→", "←↓", "↓↔",
		"↑ ", "↑→", "←↑", "↑↔",
		"↕ ", "↕→", "←↕", "↕↔"
	};
#endif

	// Initialisation
	std::vector<unsigned char> poussees = m_solv3->poussees(m_carte, m_pers->coord());
	std::vector<bool> zone    = m_solv3->zone_accessible(m_carte, m_pers->coord());
	std::vector<bool> zone_sr = m_solv3->zone_atteignable(m_carte);
	auto ref = manip::coord(5, 9);

	// Affichage des objets
	for (auto obj : *m_carte) {
		std::cout << ref + obj->coord() + manip::x * obj->coord().x();
		auto dobj = obj->get();
		Style st;

		if (dobj && m_deplacables) {
			auto pobj = std::dynamic_pointer_cast<moteur::Poussable>(dobj);

			if (pobj) {
				std::vector<bool> z = m_solv3->zone_atteignable(m_carte, pobj->coord());
				bool f = false;

				for (auto e : m_carte->liste<moteur::Emplacement>()) {
					f |= z[hash(e->coord())];
					if (f) break;
				}

				if (m_carte->get<moteur::Emplacement>(pobj->coord())) {
					st.txt(style::VERT);
				} else if (f) {
					st.txt(style::CYAN);
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
			// Style
			if (m_zone_interdite && m_solv3->zone_interdite(obj->coord())) {
				st.fnd(style::ROUGE);
			}

			if (m_stone_reachable && zone_sr[hash(obj->coord())]) {
				if (st.fnd() == style::ROUGE) {
					st.fnd(style::VIOLET);
				} else {
					st.fnd(style::CYAN);
				}
			}

			// Affichage
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << st << select_mur(m_carte, std::dynamic_pointer_cast<moteur::Obstacle>(obj));

			} else if (std::dynamic_pointer_cast<moteur::Sortie>(obj)) {
				std::cout << st << SORTIE;

			} else if (m_directions) {
				std::cout << st << fleches[m_solv3->infos_cases(obj->coord()).directions];

			} else if (m_poussees && poussees[hash(obj->coord())]) {
				std::cout << st << fleches[poussees[hash(obj->coord())]];

			} else if (m_zones_empls && m_solv3->infos_empls(obj->coord()).zone) {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::JAUNE);
				std::cout << st << "G" << m_solv3->infos_empls(obj->coord()).zone;

			} else if (m_intersections && m_solv3->infos_cases(obj->coord()).intersection) {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::VERT);
				std::cout << st << "II";

			} else if (m_tunnels && m_solv3->infos_cases(obj->coord()).tunnel) {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::VIOLET);
				std::cout << st << "TT";

			} else if (m_portes && m_solv3->infos_cases(obj->coord()).porte) {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::BLEU);
				std::cout << st << fleches[m_solv3->infos_cases(obj->coord()).porte];

			} else if (m_priorites && m_solv3->infos_empls(obj->coord()).zone) {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::CYAN);
				std::cout << st << fleches[m_solv3->infos_empls(obj->coord()).dirs()];

			} else if (std::dynamic_pointer_cast<moteur::Emplacement>(obj)) {
				st.txt(style::DEFAUT_TEXTE);
				std::cout << st << EMPL;

			} else if (m_zone_access && zone[hash(obj->coord())]) {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::CYAN);
				std::cout << st << "--";

			} else {
				std::cout << st << "  ";
			}
		}
	}

	// Reset style
	std::cout << style::defaut;
}

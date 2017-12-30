// Importations
#include <array>
#include <cmath>
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

#include "ia/chemin.hpp"
#include "ia/solveur3.hpp"

#include "affichage.hpp"
#include "devscreen.hpp"

// Constructeur
DevScreen::DevScreen(std::shared_ptr<moteur::Carte> carte)
	: hash(carte->taille_y()), m_carte(std::make_shared<moteur::Carte>(*carte)), m_chemin() {

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

	// Streams
	posstream<std::ostream> heu_stream(&std::cout, 5, 11 + m_carte->taille_y());
	posstream<std::ostream> pec_stream(&std::cout, 5, 12 + m_carte->taille_y());
	posstream<std::ostream> mvt_stream(&std::cout, 5, 13 + m_carte->taille_y());

	// Getch !
	bool continuer = true;
	int poussees = 1;
	bool maj_pec = false;

	while (continuer) {
		// Affichage !
		afficher_carte();
//		afficher_status();

		if (poussees > 0) {
			// Maj heuristique
			heu_stream << manip::eff_ligne;
			heu_stream << "Heuristique : " << m_solv3->heuristique(m_carte);

			// Maj mouvements
			mvt_stream << manip::eff_ligne;
			mvt_stream << "Mouvements  : ";

			auto mvts = m_solv3->mouvements(m_carte);
			mvts.sort(
				[] (ia::Solveur3::Mouv const& mvt1, ia::Solveur3::Mouv const& mvt2) {
					return mvt1.heuristique < mvt2.heuristique;
				}
			);

			for (auto mvt : mvts) {
				Coord pous = mvt.poussable->coord();
				Coord dest = mvt.chemin.appliquer(pous);

				mvt_stream << (char)('A' + pous.x()) << pous.y() +1 << " > " << (char)('A' + dest.x()) << dest.y() +1 << " (" << mvt.heuristique << "), ";
			}
		}

		if (poussees > 0 || maj_pec) {
			// Maj Poussable Emplacement Choix
			pec_stream << manip::eff_ligne;
			pec_stream << "PEC         : ";

			if (m_empl_conseil) {
				pec_stream << (char)('A' + m_empl_conseil->coord().x()) << m_empl_conseil->coord().y() +1 << " > ";

				Coord c = m_solv3->choix_empl(m_carte, m_empl_conseil->coord());
				pec_stream << (char)('A' + c.x()) << c.y() +1;
			} else {
				pec_stream << "nullptr";
			}

			poussees = 0;
			maj_pec = false;
		}

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

		case 'b':
			if (m_influence == Coord(-1, -1)) {
				m_influence = select_case([this] (Coord const& c) { return m_carte->get<moteur::Poussable>(c) != nullptr; });
			} else {
				m_influence = Coord(-1, -1);
			}

			break;

		case 'c':
			m_poussees = !m_poussees;
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
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

		case 'j': {
			if (m_empl_conseil) {
				m_empl_conseil = nullptr;
			} else {
				Coord p = select_case([this] (Coord const& c) { return m_carte->get<moteur::Poussable>(c) != nullptr; });
				m_empl_conseil = m_carte->get<moteur::Poussable>(p);
			}

			maj_pec = true;

			break;
		}

		case 'o': {
			Coord p = select_case([this] (Coord const& c) { return m_carte->get<moteur::Poussable>(c) != nullptr; });
			if (p != Coord(-1, -1)) {
				auto pous = m_carte->get<moteur::Poussable>(p);

				if (pous) {
					(*m_carte)[p]->pop();
					m_poussables.push_back(pous);
				}
			}

			break;
		}

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

		case 'v':
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
			m_zone_access = false;

			// Reset
			m_chemin.clear();

		{	// Calcul du chemin
			Coord dep = select_case([this] (Coord const& c) { return m_solv3->infos_cases(c).interieur; });
			if (dep == Coord(-1, -1)) break;

			Coord arr = select_case([this] (Coord const& c) { return m_solv3->infos_cases(c).interieur; });
			if (arr == Coord(-1, -1)) break;

			ia::Chemin res;
			int force = 0;

			auto pous = m_carte->get<moteur::Poussable>(dep);
			if (pous != nullptr) force = m_pers->force();

			if (m_solv3->trouver_chemin(m_carte, dep, arr, res, force)) {
				m_chemin_dep = dep;

				// Résultat
				for (auto dir : res) {
					m_chemin[dep] |= ia::get_mask(dir);
					dep += dir;
				}

				m_chemin_obj = res;
				m_aff_chemin = true;
			}
		}

			break;

		case 'w':
			m_aff_chemin = !m_aff_chemin;
			m_directions  = false;
			m_intersections = false;
			m_portes      = false;
			m_tunnels     = false;
			m_zones_empls = false;
			m_zone_access = false;
			break;

		case 'x':
			if (m_chemin_obj.longueur() != 0) {
				Coord dep = m_pers->coord();
				ia::Chemin res = m_solv3->conversion(m_carte, m_chemin_obj, m_chemin_dep, dep, m_pers->force());
				m_chemin_obj = ia::Chemin();

				// Maj
				m_chemin.clear();

				for (auto dir : res) {
					m_chemin[dep] |= ia::get_mask(dir);
					dep += dir;
				}
			}

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
		if (m_deplacables && dir != ORIGINE && m_influence == Coord(-1, -1)) {
			m_pers->deplacer(dir, poussees);
		}
	}
}

Coord DevScreen::select_case() {
	return select_case([] (Coord const&) -> bool { return true; });
}

Coord DevScreen::select_case(std::function<bool(Coord const&)> const& pred) {
	// Déclarations
	m_selection = ORIGINE;
	bool fin = false;

	// Interactions
	while (!fin) {
		// Affichage !
		afficher_carte();

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

		case 'c': // Annulation
			m_selection = Coord(-1, -1);

            #ifdef __gnu_linux__
            __attribute__((fallthrough));
            #endif

		case ENTREE:
			fin = true;
			break;
		}

		// Déplacement
		if (dir != ORIGINE) {
			m_selection += dir;
			while (m_carte->coord_valides(m_selection)) {
				if (pred(m_selection)) break;

				m_selection += dir;

				// Limites
				if (m_selection.x() < 0) {
					m_selection.x() = m_carte->taille_x()-1;
					m_selection.y()--;
				}

				if (m_selection.x() >= m_carte->taille_x()) {
					m_selection.x() = 0;
					m_selection.y()++;
				}

				if (m_selection.y() < 0) {
					m_selection.y() = m_carte->taille_y()-1;
					m_selection.x()--;
				}

				if (m_selection.y() >= m_carte->taille_y()) {
					m_selection.y() = 0;
					m_selection.x()++;
				}
			}
		}

		// Retour à l'origine
		if (!m_carte->coord_valides(m_selection)) {
			m_selection = ORIGINE;
		}
	}

	// Retour
	Coord c = m_selection;
	m_selection = Coord(-1, -1);

	return c;
}

void DevScreen::pop_poussables() {
	// Pops !
	for (auto po : m_carte->liste<moteur::Poussable>()) {
		(*m_carte)[po->coord()]->pop();
		m_poussables.push_back(po);
	}
}

void DevScreen::reset_poussables() {
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
#ifdef __gnu_linux__
	// Initialisation
    static std::map<char,std::tuple<std::function<bool(DevScreen const&)>,std::string,std::string>> options = {
		{'A', {[] (DevScreen const& ds) { return ds.m_deplacables; },            "Cacher les déplacables  ",          "Afficher les déplacables"}},
		{'B', {[] (DevScreen const&)    { return true; },                        "Montrer l'influence d'un poussable ", ""}},
		{'C', {[] (DevScreen const& ds) { return ds.m_poussees; },               "Cacher les poussees  ",             "Afficher les poussees"}},
		{'D', {[] (DevScreen const& ds) { return ds.m_portes; },                 "Cacher les portes  ",               "Afficher les portes"}},
		{'E', {[] (DevScreen const& ds) { return ds.m_priorites;  },             "Cacher les priorités  ",            "Afficher les priorités"}},
		{'F', {[] (DevScreen const& ds) { return ds.m_directions;  },            "Cacher les flèches  ",              "Afficher les flèches"}},
		{'G', {[] (DevScreen const& ds) { return ds.m_zones_empls;  },           "Cacher les zones empls  ",          "Afficher les zones empls"}},
		{'H', {[] (DevScreen const& ds) { return ds.m_intersections;  },         "Cacher les intersections  ",        "Afficher les intersections"}},
		{'I', {[] (DevScreen const& ds) { return ds.m_zone_interdite;  },        "Cacher la zone interdite  ",        "Afficher la zone interdite"}},
		{'J', {[] (DevScreen const& ds) { return ds.m_empl_conseil != nullptr; },"Cacher l'emplacement conseillé  ",  "Afficher l'emplacement conseillé"}},
		{'O', {[] (DevScreen const&)    { return true; },                        "Enlever un poussable ",             ""}},
		{'P', {[] (DevScreen const& ds) { return ds.m_poussables.size() == 0; }, "Enlever les poussables ",           "Remettre les poussables"}},
		{'S', {[] (DevScreen const& ds) { return ds.m_stone_reachable;  },       "Cacher la zone SR  ",               "Afficher la zone SR"}},
		{'T', {[] (DevScreen const& ds) { return ds.m_tunnels;  },               "Cacher les tunnels  ",              "Afficher les tunnels"}},
		{'V', {[] (DevScreen const&)    { return true; },                        "Calculer un chemin",                ""}},
		{'W', {[] (DevScreen const& ds) { return ds.m_aff_chemin; },             "Cacher le chemin  ",                "Afficher le chemin"}},
		{'Z', {[] (DevScreen const& ds) { return ds.m_zone_access;  },           "Cacher la zone accessible  ",       "Afficher la zone accessible"}},
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
#endif
}

void DevScreen::afficher_carte() const {
	// Statiques !
#ifndef __gnu_linux__
	static const std::array<std::string,16> fleches = {
		"  ", " >", " <", " x",
		" v", "v>", "v<", "vx",
		" ^", "^>", "^<", "^x",
		" x", "x>", "x<", "xx"
	};
#else
	static const std::array<std::string,16> fleches = {
		"  ", " →", " ←", " ↔",
		" ↓", "↓→", "←↓", "↓↔",
		" ↑", "↑→", "←↑", "↑↔",
		" ↕", "↕→", "←↕", "↕↔"
	};
#endif

	// Initialisation
	std::map<Coord,std::pair<Coord,Nombre<unsigned>>> assos = m_solv3->associations(m_carte);
	std::vector<bool> zone = m_solv3->zone_accessible(m_carte, m_pers->coord());
	auto ref = manip::coord(5, 9);
	std::vector<bool> zone_sr;
	Coord empl_conseil;

	if (m_influence == Coord(-1, -1)) {
		zone_sr = m_solv3->zone_sr(m_carte);
	} else {
		zone_sr = m_solv3->zone_sr(m_carte, m_influence);
	}

	if (m_empl_conseil) {
		empl_conseil = m_solv3->choix_empl(assos, m_empl_conseil->coord());
	}

	// "suppression" des poussables
	std::list<std::shared_ptr<moteur::Poussable>> poussables;
	if (m_influence != Coord(-1, -1)) {
		for (auto pous : m_carte->liste<moteur::Poussable>()) {
			if (pous->coord() != m_influence) {
				poussables.push_back(pous);
				(*m_carte)[pous->coord()]->pop();
			}
		}
	}

	std::vector<unsigned char> poussees = m_solv3->poussees(m_carte, m_pers->coord());
	std::vector<bool> zone_i = m_solv3->zone_interdite(m_carte);

	// Repères
	for (int i = 0; i < m_carte->taille_x(); ++i) {
		std::cout << ref + manip::coord(2*i, -1) << (char) ('A' + i);
	}

	for (int i = 1; i <= m_carte->taille_y(); ++i) {
		std::cout << ref + manip::coord(-floor(log10(i)) -2, i-1) << i;
	}

	// Affichage des objets
	for (auto obj : *m_carte) {
		std::cout << ref + obj->coord() + manip::x * obj->coord().x();
		auto dobj = obj->get();
		Style st = style::defaut;

		// Surlignage du curseur de sélection
		if (obj->coord() == m_selection) {
			st.effet(style::INVERSE);
		}

		// Chemin
		std::string direction = "  ";

		if (m_aff_chemin) {
			auto it = m_chemin.find(obj->coord());

			if (it != m_chemin.end()) {
				unsigned char dirs = it->second;

				// On enleve les directions inaccessibles
				if (dobj && m_deplacables && std::dynamic_pointer_cast<moteur::Poussable>(dobj)) {
					dirs = 0;

					for (auto dir : {HAUT, BAS, GAUCHE, DROITE}) {
						if (!(it->second & ia::get_mask(dir))) continue;
						if (zone[hash(obj->coord() - dir)]) dirs |= ia::get_mask(dir);
					}
				}

				direction = fleches[dirs];
			}
		}

		if (dobj && m_deplacables) {
			// Poussable ?
			auto pobj = std::dynamic_pointer_cast<moteur::Poussable>(dobj);
			if (pobj) {
				std::vector<bool> z = m_solv3->zone_sr(m_carte, pobj->coord());
				Coord empl = m_solv3->choix_empl(assos, pobj->coord());

				if (st.fnd() == style::DEFAUT_FOND) {
					if (obj->coord() == empl) {
						st.txt(style::VERT);
					} else if (empl != Coord(-1, -1) && z[hash(empl)]) {
						st.txt(style::BLEU);
					} else {
						st.txt(style::JAUNE);
					}
				}

				#ifdef __gnu_linux__
				if (pobj->poids() <= 6) {
					std::string s = BOITE;
					s.append(1, '\x7f' + pobj->poids());

					std::cout << st << direction;
					std::cout << -2 * manip::dx;
					std::cout << st << s;
				} else {
					std::cout << st << "#" << pobj->poids();
				}
				#else
				std::cout << st << BOITE << pobj->poids();
				#endif
			} else {
				if (st.fnd() == style::DEFAUT_FOND) st.txt(style::VERT);

				std::cout << st << direction;
				std::cout << -2 * manip::dx;
				std::cout << st << PERS;
			}
		} else {
			// Style
			if (st.fnd() == style::DEFAUT_FOND) {
				if (m_zone_interdite) {
					if (m_solv3->zone_interdite(obj->coord())) {
						st.fnd(style::ROUGE);
					} else if (zone_i[hash(obj->coord())]) {
						st.fnd(style::JAUNE);
					}
				}

				if (m_stone_reachable && zone_sr[hash(obj->coord())]) {
					if (st.fnd() == style::ROUGE) {
						st.fnd(style::VIOLET);
					} else if (st.fnd() == style::JAUNE) {
						st.fnd(style::VERT);
					} else {
						st.fnd(style::CYAN);
					}
				}
			}

			if (m_empl_conseil && obj->coord() == empl_conseil) st.fnd(style::BLEU);

			// Affichage
			if (std::dynamic_pointer_cast<moteur::Obstacle>(obj)) {
				std::cout << st << select_mur(m_carte, std::dynamic_pointer_cast<moteur::Obstacle>(obj));

			} else if (std::dynamic_pointer_cast<moteur::Sortie>(obj)) {
				std::cout << st << SORTIE;

			} else if (m_aff_chemin) {
				std::cout << st << direction;

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
				int nb = 0;
				for (auto p : m_solv3->infos_empls(obj->coord()).prios) {
					if ((*m_carte)[p]->accessible() || (p == m_pers->coord())) ++nb;
				}

				std::cout << st << nb << " ";

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

		// Reset style
		std::cout << style::defaut;
	}

	// retour des poussables
	for (auto pous : poussables) {
		m_carte->set(pous->coord(), pous);
	}
}

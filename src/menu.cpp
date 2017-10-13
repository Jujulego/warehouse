// Importations
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "outils.hpp"
#include "outils/coord.hpp"
#include "outils/console.hpp"
#include "outils/manip.hpp"
#include "outils/posstream.hpp"
#include "outils/style.hpp"

#include "affichage.hpp"
#include "menu.hpp"

// Paramètres
#define NB_LIGNES 15U
#define MARGE     2
#define LARGEUR   70

// Méthodes
void Menu::ajouter(std::string const& nom, std::function<void()> const& fonc) {
	auto it = m_entrees.find(nom);

	if (it != m_entrees.end()) {
		it->second.fonc = fonc;
	} else {
		m_entrees[nom] = Entree {fonc, m_entrees.size()};
	}
}

void Menu::supprimer(std::string const& nom) {
	m_entrees.erase(nom);
}

void Menu::afficher() const {
	// Initialisation
	std::vector<std::string> entrees(m_entrees.size());
	unsigned item = 0, larg = 0, nb_lignes = NB_LIGNES;
	
	// Ajout des entrées & évaluation de la largeur max des entrees
	for (auto p : m_entrees) {
		entrees[p.second.ordre] = p.first;
		larg = max(larg, p.first.size());
	}
	
	larg += 2 + MARGE;
	entrees.push_back("Quitter");
	
	// Evaluation du nombre necessaire de lignes
	nb_lignes = max((larg * entrees.size()) / (LARGEUR - larg), NB_LIGNES);
	
	if (nb_lignes > NB_LIGNES) {
		nb_lignes = entrees.size() / (entrees.size() / nb_lignes +1) +1; // optimisation
	}
	
	// Fonctions utiles
	auto calcul_pos = [&] (int i) -> auto { return manip::coord(i/nb_lignes * larg, i % nb_lignes); };
	
	posstream<std::ostream> stream(&std::cout, (LARGEUR - (larg * ((entrees.size() / nb_lignes) + 1))) / 2, 9);
	auto maj_aff = [&] () -> void {
		std::cout << manip::clear << manip::coord(0, 9 + min(entrees.size(), nb_lignes));

		// Entete
		afficher_entete(ORIGINE);

		for (unsigned i = 0; i < entrees.size(); i++) {
			// "selection"
			if (i == item) stream << style::inverse;

			stream << calcul_pos(i) << "- " << entrees[i];

			// "deselection"
			if (i == item) stream << style::noninverse;
		}
	};

	// Affichage
	maj_aff();

	// Interactions
	bool fin = false;
	while (!fin) {
		// Clavier !
		switch (console::getch()) {
		case FL_BAS:
			stream << calcul_pos(item) << "- " << entrees[item];
			item = (item + 1) % entrees.size();
			stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;

			break;

		case FL_HAUT:
			stream << calcul_pos(item) << "- " << entrees[item];

			if (item == 0) item = entrees.size();
			item--;

			stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;

			break;

		case FL_DROITE:
			if (entrees.size() > nb_lignes) {
				stream << calcul_pos(item) << "- " << entrees[item];

				item += nb_lignes;
				if (item >= entrees.size()) item %= nb_lignes;

				stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;
			}

			break;

		case FL_GAUCHE:
			if (entrees.size() > nb_lignes) {
				stream << calcul_pos(item) << "- " << entrees[item];

				if (item < nb_lignes) {
					unsigned tmp = (entrees.size() / nb_lignes) * nb_lignes;

					if (item >= entrees.size() % nb_lignes) {
						tmp -= nb_lignes;
					}
					item += tmp;
				} else {
					item -= nb_lignes;
				}

				stream << calcul_pos(item) << style::inverse << "- " << entrees[item] << style::noninverse;
			}

			break;

		case ENTREE:
			if (item == entrees.size() -1) {
				fin = true;
			} else {
				m_entrees.at(entrees[item]).fonc();
				maj_aff();
			}

			break;
		}
	}
}

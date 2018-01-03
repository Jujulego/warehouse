// Importations
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/emplacement.hpp"
#include "moteur/obstacle.hpp"
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"
#include "outils.hpp"

#include "fichierxsb.hpp"
#include "menu.hpp"
#include "niveau.hpp"

// Constructeur
FichierXSB::FichierXSB(std::string nom) {
	// Ajout de l'extension
	if (nom.size() <= 4 || nom.substr(nom.size()-4, 4) != ".xsb") {
		nom += ".xsb";
	}

	charger(nom);
}

// Méthodes
std::shared_ptr<moteur::Carte> generer_carte(std::vector<std::string> const& maze) {
    size_t larg = 0;

	for (std::string s : maze) {
        larg = std::max(s.size(), larg);
	}

	// Allocation de la carte
	auto carte = std::make_shared<moteur::Carte>(larg, maze.size());

	// Remplissage
	for (unsigned y = 0; y < maze.size(); y++) {
		for (unsigned x = 0; x < maze[y].size(); x++) {
			// Immuables
			switch (maze[y][x]) {
			case '#':
				carte->set(x, y, std::make_shared<moteur::Obstacle>(x, y, moteur::Obstacle::mur));
				break;

			case '.':
			case '*':
			case '+':
				carte->set(x, y, std::make_shared<moteur::Emplacement>(x, y));
				break;
			}

			// Deplacables
			switch (maze[y][x]) {
			case '$':
			case '*':
				carte->set(x, y, std::make_shared<moteur::Poussable>(carte.get(), 1));
				break;

			case '@':
			case '+':
				carte->set(x, y, std::make_shared<moteur::Personnage>(carte.get(), 1));
				break;
			}
		}
	}

	return carte;
}

void FichierXSB::charger(std::string const& fichier) {
	// Ouverture du fichier
	std::ifstream ifs(fichier);
	if (!ifs) return;

	// Lecture !
	std::map<std::string,std::string> infos;
	std::vector<std::string> maze;
	std::string buf;

	while (ifs) {
		std::getline(ifs, buf);

		// Commentaire
		if (buf.substr(0, 2) == "//") continue;

		// Ligne vide => fin du niveau !
		if (buf.size() == 0) {
			if (maze.size() != 0) {
				Niveau niv(generer_carte(maze));
				niv.infos() = infos;

				m_niveaux.push_back(niv);

				maze.clear();
				infos.clear();
			}

 			continue;
		}

		// Infos ....
		if ((buf[0] >= 'A' && buf[0] <= 'Z') || (buf[0] >= 'a' && buf[0] <= 'z')) {
			size_t p = buf.find(':');

			if (p != std::string::npos)
				infos[buf.substr(0, p)] = buf.substr(p+2);
		} else {
			// Labyrinthe
			maze.push_back(buf);
		}
	}
}

int FichierXSB::nb_niv() const {
	return m_niveaux.size();
}

Menu FichierXSB::as_menu() const {
	Menu menu;
	int i = 0;

	for (Niveau niv : m_niveaux) {
		i++;
		auto it = niv.infos().find("Title");

		if (it == niv.infos().end()) {
			menu.ajouter("Niveau " + to_string(i), std::bind(&Niveau::jouer, niv));
		} else {
			menu.ajouter(it->second, std::bind(&Niveau::jouer, niv));
		}
	}

	return menu;
}

std::vector<Niveau> const& FichierXSB::niveaux() const {
	return m_niveaux;
}



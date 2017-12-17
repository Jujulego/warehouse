#pragma once

// Importations
#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "outils/coord.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"
#include "ia.hpp"

// Namespace
namespace ia {

// Prototype
unsigned char get_mask(Coord const& dir);

// Classe
class Solveur3 : public IA {
	public:
		// Structures
		struct Infos {
			// Attributs
			bool coin = false;
			bool culdesac = false;
			bool tunnel = false;
			bool intersection = false;
			unsigned char porte = 0;      // Séparation entre une salle et une autre ou une salle et un tunnel (indique de quel côté de la case se trouve la porte)
			bool interieur = false;       // A l'intérieur des murs

			unsigned char directions = 0;            // Directions vers tous les emplacements
			std::map<Coord,int> distances;           // Distance aux emplacements
			std::map<Coord,unsigned char> empl_dirs; // Directions aux emplacements

			// Constructeur
			Infos(size_t factx) : distances(std::less<Coord>(factx)), empl_dirs(std::less<Coord>(factx)) {}

			// Méthodes
			void ajouter(Coord const& empl, Coord const& dir);

			bool test(Coord const& dir) const;
			bool test(Coord const& empl, Coord const& dir) const;
		};

		struct Empl {
			// Attributs
			int zone;
			Coord pos;
			std::list<Coord> prios; // Cases prioritaires sur cet emplacement

			// Méthodes
			unsigned char dirs() const;
		};

		struct Mouv {
			// Attributs
			Chemin chemin;
			unsigned heuristique;
			std::shared_ptr<moteur::Poussable> poussable;
		};

		// Constructeur
		Solveur3(std::shared_ptr<moteur::Carte> carte, std::shared_ptr<moteur::Deplacable> obj);

		// Méthodes
		virtual Chemin resoudre(posstream<std::ostream>& stream) override;

		// Outils
		// - analyse statique
		std::vector<Infos> const& infos_cases() const;
		std::vector<bool> const& zone_interdite() const;
		std::vector<Empl> const& infos_empls() const;

		Infos const& infos_cases(Coord const& c) const;
		bool zone_interdite(Coord const& c) const;
		Empl const& infos_empls(Coord const& c) const;

		// - analyse dynamique
		unsigned heuristique(std::shared_ptr<moteur::Carte> carte) const;
		Coord choix_empl(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;
		std::vector<unsigned char> poussees(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;

		std::vector<bool> zone_interdite(std::shared_ptr<moteur::Carte> carte) const;
		std::vector<bool> zone_sr(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;  // Par le poussable obj
		std::vector<bool> zone_sr(std::shared_ptr<moteur::Carte> carte) const; // "Somme" de celle de touts les poussables

		std::list<Mouv> mouvements(std::shared_ptr<moteur::Carte> carte) const; // Calculs des mouvements possibles

	private:
		// Cache
		mutable std::vector<Infos> c_infos;
		mutable std::vector<bool> c_zone_interdite;
		mutable std::vector<Empl> c_infos_empls;
};

} // ia

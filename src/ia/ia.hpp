#pragma once

// Importations
#include <atomic>
#include <future>
#include <list>
#include <memory>
#include <ostream>
#include <vector>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/poussable.hpp"
#include "outils/coord.hpp"
#include "outils/posstream.hpp"

#include "chemin.hpp"

// Namespace
namespace ia {

// Classe
class IA {
	public:
		// Constructeur
		IA(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj);
		IA(IA&& ia);

		// Destructeur
		virtual ~IA() = default;

		// Operateur
		IA& operator = (IA&& ia);

		// Méthodes virtuelles
		virtual Chemin resoudre(posstream<std::ostream>& stream) = 0;

		// Méthodes
		std::future<Chemin> async_resoudre(posstream<std::ostream>& stream);
		void interrompre();

		// Outils
		// - analyse dynamique
		std::vector<bool> zone_accessible(std::shared_ptr<moteur::Carte> carte, Coord const& obj) const;

		bool deadlock(
			std::shared_ptr<moteur::Carte> const& carte,
			std::shared_ptr<moteur::Poussable> const& pt, // L'objet à vérifier
			Coord const& pers, int force) const;          // Paramètres personnage

		bool trouver_chemin(      // Vrai = trouvé !
			std::shared_ptr<moteur::Carte> carte,
			Coord const& dep, Coord const& arr,
			Chemin& res) const;

		bool trouver_chemin(      // Vrai = trouvé !
			std::shared_ptr<moteur::Carte> carte,
			Coord const& dep, Coord const& arr,
			Chemin& res,
			int force) const; // force = 0 => pour le personnage, force != 0 => pour une boite qu'il faut pousser

	protected:
		// Attributs
		std::hash<Coord> hash;
		std::shared_ptr<moteur::Carte> m_carte;
		std::shared_ptr<moteur::Deplacable> m_obj;

		std::atomic<bool> m_interruption;

		// Outils
		std::vector<int> reduire(std::shared_ptr<moteur::Carte> const& c) const;
		std::list<Coord> mouvements(std::shared_ptr<moteur::Deplacable> const& obj) const;
		bool comparer(std::shared_ptr<moteur::Carte> const& c1, std::shared_ptr<moteur::Carte> const& c2) const;
};

} // ia

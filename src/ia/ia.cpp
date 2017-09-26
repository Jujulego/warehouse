// Importations
#include <memory>
#include <list>

#include "moteur/carte.hpp"
#include "moteur/deplacable.hpp"
#include "moteur/personnage.hpp"
#include "moteur/poussable.hpp"
#include "outils/coord.hpp"

#include "ia.hpp"

// Namespace
using namespace ia;

// Alias
template<class T, class U>
inline std::shared_ptr<T> cast(std::shared_ptr<U> const& pt) {
	return std::dynamic_pointer_cast<T>(pt);
}

// Constructeur
IA::IA(std::shared_ptr<moteur::Carte> const& carte, std::shared_ptr<moteur::Deplacable> const& obj)
	: m_carte(carte), m_obj(obj) {};

// Outils
std::list<Coord> IA::mouvements(std::shared_ptr<moteur::Deplacable> const& obj) const {
	std::list<Coord> mvts;
	
	for (Coord m : {HAUT, BAS, GAUCHE, DROITE}) {
		if (!obj->deplacer(m, true)) mvts.push_back(m);
	}
	
	return mvts;
}

bool IA::comparer(std::shared_ptr<moteur::Carte> const& c1, std::shared_ptr<moteur::Carte> const& c2) const {
	// Je suppose que les immuables sont les memes dans les 2 cartes
	bool equiv = true;
	
	for (auto pt : c1->liste<moteur::Deplacable>()) {
		// Ils faut que les 2 soient du même type
		     if (cast<moteur::Personnage>(pt) != nullptr) equiv &= (c2->get<moteur::Personnage>(pt->coord()) != nullptr);
		else if (cast<moteur::Poussable>( pt) != nullptr) equiv &= (c2->get<moteur::Poussable>( pt->coord()) != nullptr);
		
		// Break ?
		if (!equiv) break;
	}
	
	return equiv;
}

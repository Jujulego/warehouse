#pragma once

// Importations
#include <ostream>

#include "coord.hpp"

// -------------- BaseManip ------------
class BaseManip {
	public:
		// Constructeur
		BaseManip() = default;
		
		// Destructeur
		virtual ~BaseManip() = default;
		
		// Regle de 5
		BaseManip(BaseManip const&) = default;
		BaseManip(BaseManip&&)      = default;
		BaseManip& operator = (BaseManip const&) = default;
		BaseManip& operator = (BaseManip&&)      = default;
		
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const = 0;
};

// Opérateur externe
std::ostream& operator << (std::ostream& stream, BaseManip const& manip);

// ------------- ClearManip ------------
class ClearManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ------------- CoordManip ------------
class CoordManip : public BaseManip, public Coord {
	public:
		// Constructeur
		CoordManip(Coord const& c);
		CoordManip(int x = 0, int y = 0);

		// Opérateurs
		CoordManip  operator +  () const;
		CoordManip  operator -  () const;
		CoordManip  operator +  (Coord const& c) const;
		CoordManip  operator -  (Coord const& c) const;
		CoordManip  operator *  (int k)          const;
		CoordManip  operator /  (int k)          const;
		CoordManip& operator += (Coord const& c);
		CoordManip& operator -= (Coord const& c);
		CoordManip& operator *= (int k);
		CoordManip& operator /= (int k);

		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// Opérateur externe
CoordManip operator * (int const& k, CoordManip const& cm);

// ------------- MouvManip -------------
class MouvManip : public CoordManip {
	public:
		// Constructeur
		MouvManip(Coord const& c);
		MouvManip(int dx, int dy);

		// Opérateurs
		MouvManip  operator +  () const;
		MouvManip  operator -  () const;
		MouvManip  operator +  (Coord const& c) const;
		MouvManip  operator -  (Coord const& c) const;
		MouvManip  operator *  (int k)          const;
		MouvManip  operator /  (int k)          const;
		MouvManip& operator += (Coord const& c);
		MouvManip& operator -= (Coord const& c);
		MouvManip& operator *= (int k);
		MouvManip& operator /= (int k);

		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// Opérateur externe
MouvManip operator * (int const& k, MouvManip const& mm);

// ----------- EffLigneManip -----------
class EffLigneManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ------------ SauveManip -------------
class SauveManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ----------- RestoreManip ------------
class RestoreManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// ------------- BuzzManip -------------
class BuzzManip : public BaseManip {
	public:
		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;
};

// Instances
namespace manip {
	// Basiques
	static ClearManip clear;         // Efface l'écran

	// Coords
	static CoordManip origine(0, 0); // Le point en haut à gauche
	static CoordManip x(1, 0);
	static CoordManip y(0, 1);
	inline CoordManip coord(int x, int y) { return CoordManip(x, y); }
	inline CoordManip coord(Coord c)      { return CoordManip(c.x(), c.y()); }

	// Basiques
	static EffLigneManip eff_ligne;  // Efface la ligne
	static SauveManip    sauve;      // Sauvegarde la position du curseur
	static RestoreManip  restore;    // Restore la dernière position sauvegardée
	static BuzzManip     buzz;       // Fait buzzer l'ordi

	// Mouv
	static MouvManip dx(1, 0);       // Décalage du curseur
	static MouvManip dy(0, 1);
};

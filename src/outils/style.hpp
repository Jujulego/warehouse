#pragma once

// Importations
#include <ostream>

#include "manip.hpp"

namespace style {
	// Enumérations
	enum Couleur {
		// Couleurs
		ACTUELLE = 0xffff,
		NOIR     = 0x00,
		ROUGE    = 0x01,
		VERT     = 0x02,
		JAUNE    = 0x03,
		BLEU     = 0x04,
		VIOLET   = 0x05,
		CYAN     = 0x06,
		BLANC    = 0x07,

		// Par défaut
		DEFAUT_TEXTE = 0x08,
		DEFAUT_FOND  = 0x09,
	};

	enum Position {
		// Position
		TEXTE = 0x30,
		FOND  = 0x40
	};

	enum Effet : int{
		// Type
		NORMAL     = 0x00,
		GRAS       = 0x01,
		SOMBRE     = 0x02,
		ITALIQUE   = 0x03,
		SOULIGNE   = 0x04,
		CLIGNOTANT = 0x05,
		INVERSE    = 0x07, // les couleurs du fond et du texte
		INVISIBLE  = 0x08,
		BARRE      = 0x09,

		// Négation
		NON_GRAS       = 0x21,
		NON_SOMBRE     = 0x22,
		NON_ITALIQUE   = 0x23,
		NON_SOULIGNE   = 0x24,
		NON_CLIGNOTANT = 0x25,
		NON_INVERSE    = 0x27, // les couleurs du fond et du texte
		NON_INVISIBLE  = 0x28,
		NON_BARRE      = 0x29,
	};
}

// Classe :
class Style : public BaseManip {
	private:
		// Attributs
		style::Couleur m_txt = style::ACTUELLE;
		style::Couleur m_fnd = style::ACTUELLE;
		style::Effet m_effet = style::NORMAL;

	public:
		// Constructeurs
		constexpr Style(style::Couleur txt, style::Couleur fnd, style::Effet effet) : m_txt(txt), m_fnd(fnd), m_effet(effet) {};
		Style(style::Couleur coul, style::Position pos, style::Effet effet) : m_effet(effet) {
			switch (pos) {
			case style::TEXTE:
				m_txt = coul;
				break;

			case style::FOND:
				m_fnd = coul;
				break;
			}
		}

		Style(style::Couleur coul, style::Position pos) : Style(coul, pos, style::NORMAL) {};
		Style(style::Couleur txt, style::Couleur fnd) : Style(txt, fnd, style::NORMAL) {};
		Style(style::Effet effet) : Style(style::ACTUELLE, style::ACTUELLE, effet) {};
		Style() : Style(style::DEFAUT_TEXTE, style::DEFAUT_FOND, style::NORMAL) {};

		// Méthodes
		virtual std::ostream& appliquer(std::ostream& stream) const override;

		// Accesseurs
		style::Couleur txt() const;
		style::Couleur fnd() const;
		style::Effet effet() const;

		void txt(style::Couleur const& txt);
		void fnd(style::Couleur const& fnd);
		void effet(style::Effet const& effet);
};

// Valeurs
namespace style {
	// Retour par défaut (texte blanc + fond noir)
	static const Style defaut;

	// Couleur du texte
	static const Style noir(  NOIR,   TEXTE);
	static const Style rouge( ROUGE,  TEXTE);
	static const Style vert(  VERT,   TEXTE);
	static const Style jaune( JAUNE,  TEXTE);
	static const Style bleu(  BLEU,   TEXTE);
	static const Style violet(VIOLET, TEXTE);
	static const Style cyan(  CYAN,   TEXTE);
	static const Style blanc( BLANC,  TEXTE);

	// Couleur du fond
	static const Style fnoir(  NOIR,   FOND);
	static const Style frouge( ROUGE,  FOND);
	static const Style fvert(  VERT,   FOND);
	static const Style fjaune( JAUNE,  FOND);
	static const Style fbleu(  BLEU,   FOND);
	static const Style fviolet(VIOLET, FOND);
	static const Style fcyan(  CYAN,   FOND);
	static const Style fblanc( BLANC,  FOND);

	// Style (seuls "sombre" et "inverse" sont supporté par Windows)
	static const Style gras(    GRAS);
	static const Style sombre(  SOMBRE);
	static const Style italique(ITALIQUE);
	static const Style souligne(SOULIGNE);
	static const Style inverse( INVERSE);  // Inverse les couleurs de fond et du texte
	static const Style barre(   BARRE);

	// Anti-Style
	static const Style nongras(    NON_GRAS);
	static const Style nonsombre(  NON_SOMBRE);
	static const Style nonitalique(NON_ITALIQUE);
	static const Style nonsouligne(NON_SOULIGNE);
	static const Style noninverse( NON_INVERSE);
	static const Style nonbarre(   NON_BARRE);

	// Styles prédéfini
	static const Style debug(   BLEU,  TEXTE);
	static const Style info(    BLANC, TEXTE, GRAS);
	static const Style warning( JAUNE, TEXTE);
	static const Style erreur(  ROUGE, TEXTE);
	static const Style critique(ROUGE, TEXTE, GRAS);
}

// Opérateurs externes
std::ostream& operator << (std::ostream& stream, Style const& style);

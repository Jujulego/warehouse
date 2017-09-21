#pragma once

// Importations
#include <string>

// Min et Max
template<class T>
T min(T const& t1, T const& t2) {
	return t1 < t2 ? t1 : t2;
}

template<class T>
T max(T const& t1, T const& t2) {
	return t1 > t2 ? t1 : t2;
}

// Caractètres
/* Le nom des constantes contient les directions incluses au charactère :
 *     ex : la barre horizontale : TAB_DG
 *
 * Directions :
 *   H
 * G   D
 *   B
 */

#ifndef __gnu_linux__ // Pour Windows
// Tableau
static const std::string TAB_HB   = "\xb3";
static const std::string TAB_DG   = "\xc4";
static const std::string TAB_HD   = "\xc0";
static const std::string TAB_DB   = "\xda";
static const std::string TAB_BG   = "\xbf";
static const std::string TAB_GH   = "\xd9";
static const std::string TAB_HDB  = "\xc3";
static const std::string TAB_DBG  = "\xc2";
static const std::string TAB_BGH  = "\xb4";
static const std::string TAB_GHD  = "\xc1";
static const std::string TAB_HDBG = "\xc5";

// Touches
static const std::string ENTREE    = "\r";
static const std::string FL_DROITE = "\xe0M";
static const std::string FL_GAUCHE = "\xe0K";
static const std::string FL_HAUT   = "\xe0H";
static const std::string FL_BAS    = "\xe0P";

#else // Pour Linux
// Tableau
static const std::string TAB_HB   = "\xe2\x94\x82";
static const std::string TAB_DG   = "\xe2\x94\x80";
static const std::string TAB_HD   = "\xe2\x95\xb0";
static const std::string TAB_DB   = "\xe2\x95\xad";
static const std::string TAB_BG   = "\xe2\x95\xae";
static const std::string TAB_GH   = "\xe2\x95\xaf";
static const std::string TAB_HDB  = "\xe2\x94\x9c";
static const std::string TAB_DBG  = "\xe2\x94\xac";
static const std::string TAB_BGH  = "\xe2\x94\xa4";
static const std::string TAB_GHD  = "\xe2\x94\xb4";
static const std::string TAB_HDBG = "\xe2\x94\xbc";

// Touches
static const std::string ENTREE    = "\n"
static const std::string FL_DROITE = "\x1b[C"
static const std::string FL_GAUCHE = "\x1b[D"
static const std::string FL_HAUT   = "\x1b[A"
static const std::string FL_BAS    = "\x1b[B"
#endif

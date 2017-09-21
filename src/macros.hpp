#pragma once

// Min et Max
#ifndef MIN
#define MIN(x1, x2) ((x1) < (x2) ? (x1) : (x2))
#endif

#ifndef MAX
#define MAX(x1, x2) ((x1) < (x2) ? (x2) : (x1))
#endif

// Paramètres
// Y en a pas !

// Caractètres
/* Le nom des constantes contient les directions incluses au charactère :
 *     ex : la barre horizontale : TAB_DG
 *
 * Directions :
 *   H
 * G   D
 *   B
 */

#include <string>
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
# define ENTREE    13
# define FL_DROITE 57421
# define FL_GAUCHE 57419
# define FL_HAUT   57416
# define FL_BAS    57424

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

// Commandes
# define ENTREE    10
# define FL_DROITE 1792835
# define FL_GAUCHE 1792836
# define FL_HAUT   1792833
# define FL_BAS    1792834
#endif

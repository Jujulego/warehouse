#pragma once

// Imporations
#include <algorithm>
#include <set>
#include <vector>

#include "coord.hpp"
#include "matrice.hpp"
#include "nombre.hpp"

// Algorithme Hongrois : https://fr.wikipedia.org/wiki/Algorithme_hongrois
template<class Int>
std::set<Coord> algorithme_hongrois(Matrice<Int> mat) {
	const auto LIG = mat.nb_lig();

	std::less<Coord> less(LIG);
	std::set<Coord> selection(less);
	std::set<Coord> primes(less);
	std::set<int> lig_sel, col_sel;
	std::set<int> lig_couv, col_couv;

	// La matrice doit être carrée
	if (mat.nb_col() != LIG) return selection;

	// Etape 0
	for (auto ligne : mat.lignes()) {
		Int min = *std::min_element(ligne.begin(), ligne.end());

		for (Int& v : ligne) {
			v -= min;
		}
	}

	for (auto colonne : mat.colonnes()) {
		Int min = *std::min_element(colonne.begin(), colonne.end());

		for (Int& v : colonne) {
			v -= min;
		}
	}

	while (true) {
		// Etape 1
		int anc_taille = selection.size();

		for (int col = 0; col < LIG; ++col) {
			// Colonne déjà sélectionnée ?
			if (col_sel.find(col) != col_sel.end()) continue;

			for (int lig = 0; lig < LIG; ++lig) {
				// Ligne déjà sélectionnée ?
				if (lig_sel.find(lig) != lig_sel.end()) continue;

				// = 0 ?
				Coord z(lig, col);
				if (mat[z] == 0) {
					selection.insert(z);

					col_sel.insert(col);
					lig_sel.insert(lig);
					break;
				}
			}
		}

		if (selection.size() == LIG) break;
		if (selection.size() > anc_taille) {
			primes.clear();
			lig_couv.clear();
			col_couv.clear();
		}

		// Etape 1.5
		for (int j : col_sel) {
			col_couv.insert(j);
		}

		for (int i : lig_sel) {
			lig_couv.erase(i);
		}

		// Etape 2
		Coord zero_prime_non_couv;
		bool prime = false;
		bool check = true;

		do {
			check = false;

			for (int j = 0; j < LIG; ++j) {
				for (int i = 0; i < LIG; ++i) {
					Coord ij(i, j);

					// pas couvert
					if (lig_couv.find(i) != lig_couv.end()) continue;
					if (col_couv.find(j) != col_couv.end()) break;

					if (mat[ij] == 0) {
						primes.insert(ij);

						if (lig_sel.find(i) != lig_sel.end()) {
							lig_couv.insert(i);

							for (Coord z : selection) {
								if (z.x() != i) continue;

								col_couv.erase(z.y());
								check = true;
								break;
							}

						} else {
							zero_prime_non_couv = ij;
							prime = true;
							break;
						}
					}
				}

				if (prime) break;
			}

			if (prime) break;
		} while (check);

		// Etape 3
		if (prime) {
			// Etape 3'
			std::vector<Coord> z = { zero_prime_non_couv };

			while (true) {
				if (z.size() % 2) { // impair -> zero sélectionné meme colonne
					// N'est pas sélectionnée ?
					if (col_sel.find(z.back().y()) == col_sel.end()) break;

					// Ajout du zero à la suite
					for (Coord zero : selection) {
						if (zero.y() == z.back().y()) {
							z.push_back(zero);
							break;
						}
					}

				} else { // pair -> zero' meme ligne
					// Ajout du zero à la suite
					for (Coord zero : primes) {
						if (zero.x() == z.back().x()) {
							z.push_back(zero);
							break;
						}
					}
				}
			}

			// Sélecetions et dé-sélections
			for (int i = 0; i < z.size(); ++i) {
				if (i % 2) {
					// Pas besoin de supprimer => la ligne & la colonne seront reprises
					selection.erase(z[i]);
				} else {
					selection.insert(z[i]);

					lig_sel.insert(z[i].x());
					col_sel.insert(z[i].y());
				}
			}

			primes.clear();
			lig_couv.clear();
			col_couv.clear();
		} else {
			// Etape 3
			Int lambda = std::numeric_limits<Int>::max();

			for (int j = 0; j < LIG; ++j) {
				for (int i = 0; i < LIG; ++i) {
					Coord ij(i, j);

					// pas couvert
					if (lig_couv.find(i) != lig_couv.end()) continue;
					if (col_couv.find(j) != col_couv.end()) break;

					// Calcul du minimum
					lambda = std::min(lambda, mat[ij]);
				}
			}

			for (int lig : lig_couv) {
				for (Int& v : mat.ligne(lig)) v += lambda;
			}

			for (int col = 0; col < LIG; ++col) {
				// pas couvert
				if (col_couv.find(col) != col_couv.end()) continue;

				for (Int& v : mat.colonne(col)) v -= lambda;
			}
		}
	}

	return selection;
}


// Importations
#include <vector>

#include "hashtable.hpp"

// Namespace
using namespace ia;

// Outils
size_t Hash::operator () (std::vector<int> const& r) const {
	size_t resultat = 0;

	for (unsigned i = r.size(); i > 0; --i) {
		resultat += i * r[i-1];
	}

	return resultat;
}

bool Egal::operator () (std::vector<int> const& r1, std::vector<int> const& r2) const {
	if (r1.size() != r2.size()) return false;

	for (unsigned i = 0; i < r1.size(); ++i) {
		if (r1[i] != r2[i]) return false;
	}

	return true;
}

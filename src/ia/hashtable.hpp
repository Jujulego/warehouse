#pragma once

// Importations
#include <unordered_set>
#include <vector>

// Namespace
namespace ia {

// Outils
struct Hash {
	// Opérateur d'appel
	size_t operator () (std::vector<int> const& r) const;
};

struct Egal {
	// Opérateur d'appel
	bool operator () (std::vector<int> const& r1, std::vector<int> const& r2) const;
};

// HashTable !
using HashTable = std::unordered_set<std::vector<int>,Hash,Egal>;

} // ia

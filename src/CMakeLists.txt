#
# Ajout de l'executable
#
add_executable(warehouse
	# Moteur de jeu
	moteur/carte.hpp
	moteur/carte.cpp
	moteur/objet.hpp
	moteur/objet.cpp
	
	# Executable !
	macros.hpp
	main.cpp
	
	# Outils
	outils/console.cpp
	outils/console.hpp
	outils/coord.hpp
	outils/coord.cpp
	outils/manip.cpp
	outils/manip.hpp
	outils/style.cpp
	outils/style.hpp
)

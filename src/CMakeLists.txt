

#
# Ajout de l'executable
#
add_executable(warehouse
	# Moteur de jeu
	moteur/carte.hpp
	moteur/carte.cpp
	moteur/deplacable.hpp
	moteur/deplacable.cpp
	moteur/emplacement.hpp
	moteur/emplacement.cpp
	moteur/immuable.hpp
	moteur/immuable.cpp
	moteur/objet.hpp
	moteur/objet.cpp
	moteur/obstacle.hpp
	moteur/obstacle.cpp
	moteur/personnage.hpp
	moteur/personnage.cpp
	moteur/poussable.hpp
	moteur/poussable.cpp
	moteur/sortie.hpp
	moteur/sortie.cpp

	# IAs
	ia/ia.hpp
	ia/ia.cpp
	ia/chemin.hpp
	ia/chemin.cpp
	ia/hashtable.hpp
	ia/hashtable.cpp
	ia/noeud.hpp
	ia/noeud.cpp
	ia/solveur.hpp
	ia/solveur.cpp
	ia/solveur2.hpp
	ia/solveur2.cpp
	ia/solveur3.hpp
	ia/solveur3.cpp

	# Executable !
	affichage.hpp
	affichage.cpp
	devscreen.hpp
	devscreen.cpp
	fichierxsb.cpp
	fichierxsb.hpp
	main.cpp
	menu.hpp
	menu.cpp
	niveau.hpp
	niveau.cpp

	# Outils
	outils.hpp
	outils/console.cpp
	outils/console.hpp
	outils/coord.hpp
	outils/coord.cpp
	outils/hongrois.hpp
	outils/infini.hpp
	outils/infini.cpp
	outils/manip.cpp
	outils/manip.hpp
	outils/matrice.hpp
	outils/nombre.hpp
	outils/style.cpp
	outils/style.hpp
)

#
# Librairies
#
target_link_libraries(warehouse
	${CMAKE_THREAD_LIBS_INIT}
)

if (QT_ENABLE)
	target_link_libraries(warehouse
		Qt5::Widgets
		Qt5::Multimedia
	)

	set_target_properties(warehouse
		PROPERTIES
			AUTOMOC ON
			AUTORCC ON
	)

	target_sources(warehouse
		PUBLIC ../res/tileset/res-iso.qrc
		PUBLIC ../res/images/images.qrc

		PUBLIC qt/fenetremenu.h
		PUBLIC qt/fenetremenu.cpp
		PUBLIC qt/fenetreniveau.h
		PUBLIC qt/fenetreniveau.cpp
		PUBLIC qt/fenetreregles.h
		PUBLIC qt/fenetreregles.cpp

		PUBLIC qt/qia.hpp
		PUBLIC qt/qia.cpp
	)

#	add_definitions(-DQTCREATOR)
	add_definitions(-DQT_DEPRECATED_WARNINGS)
endif()

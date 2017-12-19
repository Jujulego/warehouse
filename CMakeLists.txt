#
# Options Générales
#
cmake_minimum_required(VERSION 3.5)

project(warehouse)

#
# Options
#
find_package(Doxygen)
option(CLANG_ANALYSE "Analyse statique (Clang)."      ON)
option(DOCUMENTATION "Crée et installe une API de documentation (Doxygen)." ON)
option(GRAPHE_APPEL  "Génère un graphe d'appel."      ON)
option(QT_ENABLE     "Compile l'interface graphique." ON)

#
# Paramètres
#
set(CMAKE_AUTOMOC               OFF)
set(CMAKE_AUTORCC               OFF)
set(CMAKE_INCLUDE_CURRENT_DIR   ${QT})
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
set(QT_RESOURCE_DIR   "${PROJECT_SOURCE_DIR}/res")

# Commandes
set(CLANG_C_PARSER   clang)
set(CLANG_CXX_PARSER clang++)
set(CLANG_OPT_PARSER opt)

if (UNIX)
	set(LIBRARY_OUTPUT_PATH           "${CMAKE_CURRENT_SOURCE_DIR}/lib")
	set(EXECUTABLE_OUTPUT_PATH        "${CMAKE_CURRENT_SOURCE_DIR}")
else()
	set(LIBRARY_OUTPUT_PATH    "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif()

#
# Options
#
set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-sign-compare -g3")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Dépendances
#
# Threads
find_package(Threads REQUIRED)

# Qt5
if (QT_ENABLE)
	find_package(Qt5 5.7 REQUIRED
		COMPONENTS
			Widgets
	)
endif()

#
# Compilation des sources
#
include_directories(src)
add_subdirectory(src)

if (QT_ENABLE)
	include(qtcreator)
	generer_pro(warehouse)
else()
	include(codeblocks)
	generer_cbp(warehouse)
endif()

#
# Cibles custom
#
# Documentation
if (DOCUMENTATION)
	include(doxygen)
endif()

# Analyse statique clang
if (CLANG_ANALYSE)
	include(clang)
	analyse_statique_clang(${CMAKE_CURRENT_BINARY_DIR}/analyse)
endif()

# Graphe d'appel
if (GRAPHE_APPEL)
	include(graphe)

    # Détections des sources
    add_graphe_appel(warehouse)
endif()

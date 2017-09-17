#
# Documentation
#

# Gardien
if (NOT DOXYGEN_FOUND)
	message(FATAL_ERROR "Doxygen est obligatoire pour créer la documentation.")
endif()

# Listes de dossiers ...
get_property(list_incdir TARGET warehouse PROPERTY INCLUDE_DIRECTORIES)
string(REPLACE ";" " " INCLUDE_DIRECTORIES "${list_incdir}")

list(REMOVE_ITEM list_incdir ${PROJECT_SOURCE_DIR})
string(REPLACE ";" " " EXCLUDE_DIRECTORIES "${list_incdir}")

# Configuration de Doxygen
set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/tpl/Doxyfile.in)
set(doxyfile    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${doxyfile_in} ${doxyfile} @ONLY)

# cible !
add_custom_target(docs
	COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Génération de la documentation (Doxygen)"
	VERBATIM
)

# Installation
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)

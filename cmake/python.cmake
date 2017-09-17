#
# Gestion de module
#
function(pymod nom)
	# Analyse arguments
	set(options)
	set(oneValueArgs)
	set(multiValueArgs SOURCES DEPENDS)
	cmake_parse_arguments(PYMOD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	# Librairie
	add_library(${nom} SHARED
		${PYMOD_SOURCES}
	)
	
	set_target_properties(${nom}
		PROPERTIES
			PREFIX ""
	)
	
	# Dépendances
	link_boost(${nom})
	
	list(LENGTH PYMOD_DEPENDS len)
	if(NOT (${len} EQUAL 0))
		target_link_libraries(${nom} ${PYMOD_DEPENDS})
	endif()
endfunction()

#
# Gestion de package
#
function(pypkg nom dossier)
	# Analyse arguments
	set(options)
	set(oneValueArgs   INIT)
	set(multiValueArgs SOURCES MODULES SOUSPKG)
	cmake_parse_arguments(PYPKG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	# Dossier
	set(pkg ${dossier}/${nom})
	file(MAKE_DIRECTORY ${pkg})
	
	# Fichier __init__.py
	if(${PYPKG_INIT} STREQUAL "")
		add_custom_command(
			OUTPUT  ${pkg}/__init__.py
			COMMAND touch ${pkg}/__init__.py
		)
	else()
		add_custom_command(
			OUTPUT  ${pkg}/__init__.py
			COMMAND cp -p ${CMAKE_CURRENT_SOURCE_DIR}/${PYPKG_INIT} ${pkg}/__init__.py
			DEPENDS ${PYPKG_INIT}
		)
	endif()
	
	# Ajout des librairies
	foreach(lib ${PYPKG_MODULES})
		set_target_properties(${lib}
			PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY ${pkg}
		)
	endforeach(lib)
	
	# Copie des sources
	set(sources)
	foreach(src ${PYPKG_SOURCES})
		# Check type de source
		string(REGEX MATCH ".+\.py$" PY ${src})
		if (NOT PY)
			message(WARNING "'${src}' n'est pas une source python !")
			continue()
		endif()
		
		# Copie
		add_custom_command(
			OUTPUT  ${pkg}/${src}
			COMMAND cp -p ${CMAKE_CURRENT_SOURCE_DIR}/${src} ${pkg}/${src}
			DEPENDS ${src}
		)
		
		list(APPEND sources "${pkg}/${src}")
	endforeach(src)
	
	# Ajout des py_ devant les sous packets
	set(souspkg)
	foreach(p ${PYPKG_SOUSPKG})
		list(APPEND souspkg "py_${nom}")
	endforeach(p)
	
	# Ajout de la cible générale
	add_custom_target("py_${nom}" ALL
		DEPENDS ${pkg}/__init__.py ${PYPKG_MODULES} ${sources} ${souspkg}
	)
endfunction()

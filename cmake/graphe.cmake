#
# Graphe d'appel
#

# Paramètres
message(STATUS "Préparation graphe d'appel")
set(GRAPHE_APPEL_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/graphe_appel)

# Commandes
set(CXXFILTER c++filt)
set(PYTHON3   python3)

# Préparation fichier sed
set(sed_file_in ${CMAKE_CURRENT_SOURCE_DIR}/tpl/sed.txt.in)
set(sed_file    ${GRAPHE_APPEL_BASE_DIR}/sed.txt)
configure_file(${sed_file_in} ${sed_file} @ONLY)

# Configuration du script python
set(dotpy_in ${CMAKE_CURRENT_SOURCE_DIR}/tpl/dot.py.in)
set(dotpy    ${GRAPHE_APPEL_BASE_DIR}/dot.py)
configure_file(${dotpy_in} ${dotpy} @ONLY)

# Fonction générale
function(add_graphe_appel target)
	message(STATUS "Préparation graphe d'appel ${target}")
	
	# Initialisation
	set(GRAPHE_APPEL_DIR ${GRAPHE_APPEL_BASE_DIR}/${nom})
	set(dots)
	
	# Création des cibles (analyse clang)
	get_target_property(nom     ${target} NAME)
	get_target_property(src_dir ${target} SOURCE_DIR)
	get_target_property(srcs    ${target} SOURCES)
	
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" src_dir "${src_dir}")
	
	foreach(src ${srcs})
		string(CONCAT src ${src_dir}/${src})
		
		# Test language (c / cpp)
		string(REGEX MATCH "\.c$"   C_SRC   ${src})
		string(REGEX MATCH "\.cpp$" CXX_SRC ${src})
		
		if(NOT (C_SRC OR CXX_SRC))
#			message(WARNING "'${src}' n'est ni une source C (.c), ni une soure C++ (.cpp)")
			continue() # Suivant !
		endif()
		
		# Noms de fichiers
		string(CONCAT base ${GRAPHE_APPEL_DIR}/${src})
		string(REGEX REPLACE "/[^/]+$" "" dir ${base})
		file(MAKE_DIRECTORY ${dir})
		
		set(llvm)
		set(dot)
		string(APPEND llvm ${base} .ll)
		string(APPEND dot  ${base} .dot)
		list(APPEND dots ${dot})
		
		# Assembleur LLVM
		if (C_SRC)
			add_custom_command(
				OUTPUT  ${llvm}
				COMMAND ${CLANG_C_PARSER} -Wall -Wextra -S -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/${src} -I ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir} -o ${llvm}
				MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${src}
			)
		elseif (CXX_SRC)
			add_custom_command(
				OUTPUT  ${llvm}
				COMMAND ${CLANG_CXX_PARSER} -Wall -Wextra -std=c++14 -S -emit-llvm ${CMAKE_CURRENT_SOURCE_DIR}/${src} -I ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir} -o ${llvm}
				MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${src}
			)
		endif()
		
		# Génération du .dot
		add_custom_command(
			OUTPUT  ${dot}
			COMMAND ${CLANG_OPT_PARSER} -analyze -std-link-opts -dot-callgraph ${llvm} > /dev/null 2> /dev/null
			COMMAND cat callgraph.dot | ${PYTHON3} ${dotpy} | ${CXXFILTER} -nip | sed -f ${sed_file} > ${dot}
			MAIN_DEPENDENCY ${llvm}
		)
	endforeach(src)
	
	# Graphe complet
	add_custom_command(
		OUTPUT  ${GRAPHE_APPEL_DIR}/graphe.dot
		COMMAND ${PYTHON3} ${dotpy} ${dots} > ${GRAPHE_APPEL_DIR}/graphe.dot
		DEPENDS ${dots}
	)
	
	add_custom_target(${nom}.svg
		COMMAND dot ${GRAPHE_APPEL_DIR}/graphe.dot -Tsvg -o ${CMAKE_SOURCE_DIR}/${nom}.svg
		DEPENDS ${GRAPHE_APPEL_DIR}/graphe.dot
	)
	
	add_custom_target(${nom}.png
		COMMAND dot ${GRAPHE_APPEL_DIR}/graphe.dot -Tpng -o ${CMAKE_SOURCE_DIR}/${nom}.png
		DEPENDS ${GRAPHE_APPEL_DIR}/graphe.dot
	)
	
	# Fini !
	message(STATUS "Préparation graphe d'appel ${target} - done")
endfunction()

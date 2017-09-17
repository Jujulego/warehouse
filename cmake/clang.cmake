#
# Analyse statique clang
#
function(analyse_statique_clang ANALYSE_DIR)
	message(STATUS "Préparation analyse statique")
	
	# Include paths
	get_property(list_incdir TARGET warehouse PROPERTY INCLUDE_DIRECTORIES)
	list(APPEND list_incdir ${Qt5Widgets_INCLUDE_DIRS})
	string(REPLACE ";" ";-I" INCLUDE_DIRECTORIES "${list_incdir}")
	
	# Sources
	file(GLOB_RECURSE analyse_srcs ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.hpp)
	set(htmls)
	
	foreach(src ${analyse_srcs})
		# Test language (c / cpp)
		string(REGEX MATCH "\.[ch]$"   C_SRC   ${src})
		string(REGEX MATCH "\.[ch]pp$" CXX_SRC ${src})
		
		if(NOT (C_SRC OR CXX_SRC))
			message(WARNING "'${src}' n'est ni une source C (.c, .h), ni une soure C++ (.cpp, .hpp)")
			continue() # Suivant !
		endif()
		
		# Noms de fichiers
		string(REPLACE ${PROJECT_SOURCE_DIR}  ${ANALYSE_DIR} base ${src})
		string(REPLACE ${PROJECT_SOURCE_DIR}/ ""             aff  ${src})
		string(REGEX REPLACE "/[^/]+$" "" dir ${base})
		file(MAKE_DIRECTORY ${dir})
		
		set(html)
		string(APPEND html ${base} .html)
		list(APPEND htmls ${html})
		
		# Analyse Clang
		if(C_SRC)
			add_custom_command(
				OUTPUT  ${html}
				COMMAND ${CLANG_C_PARSER} -Wall -Wextra --analyze ${src} -I${INCLUDE_DIRECTORIES} -o ${html}
				MAIN_DEPENDENCY ${src}
				COMMENT "Analyse de ${aff}"
			)
		elseif(CXX_SRC)
			add_custom_command(
				OUTPUT  ${html}
				COMMAND ${CLANG_CXX_PARSER} -Wall -Wextra -std=c++14 -fPIC --analyze ${src} -I${INCLUDE_DIRECTORIES} -o ${html}
				MAIN_DEPENDENCY ${src}
				COMMENT "Analyse de ${aff}"
			)
		endif()
	endforeach(src)
	
	add_custom_target("analyse"
		DEPENDS ${htmls}
		COMMENT "Analyse statique terminée !"
	)
	
	# Fini !
	message(STATUS "Préparation analyse statique - done")
endfunction(analyse_statique_clang)

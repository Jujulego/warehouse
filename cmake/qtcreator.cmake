#
# Génération projet Codeblocks
#

# Fonction générale
function(generer_pro target)
	message(STATUS "Génération ${target}.pro")
	
	# Récupération de la liste des sources
	get_target_property(nom     ${target} NAME)
	get_target_property(src_dir ${target} SOURCE_DIR)
	get_target_property(srcs    ${target} SOURCES)
	
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" src_dir "${src_dir}")
	
	# Texte pour les sources
	set(HEADERS)
	set(SOURCES)
	
	foreach(src ${srcs})
        string(REGEX MATCH "\.h(pp)?$" est_header ${src})
#		get_source_file_property(est_header "${src_dir}/${src}" HEADER_FILE_ONLY)
		
		if (est_header)
			string(APPEND HEADERS "${src_dir}/${src} \\\n\t")
		else()
			string(APPEND SOURCES "${src_dir}/${src} \\\n\t")
		endif()
	endforeach(src)
	
	# Fichier CodeBlocks !
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tpl/${nom}.pro.in
		${CMAKE_CURRENT_SOURCE_DIR}/${nom}.pro
	)
	
	# Fini !
	message(STATUS "Génération ${target}.pro - done")
endfunction()

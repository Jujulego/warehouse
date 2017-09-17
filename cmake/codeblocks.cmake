#
# Génération projet Codeblocks
#

# Fonction générale
function(generer_cbp target)
	message(STATUS "Préparation fichier cbp ${target}")
	
	# Récupération de la liste des sources
	get_target_property(nom     ${target} NAME)
	get_target_property(src_dir ${target} SOURCE_DIR)
	get_target_property(srcs    ${target} SOURCES)
	
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" src_dir "${src_dir}")
	
	# Texte pour les sources
	set(SOURCES)
	
	foreach(src ${srcs})
		string(APPEND SOURCES "<Unit filename=\"" ${src_dir}/${src} "\" />\n\t\t")
	endforeach(src)
	
	# Fichier CodeBlocks !
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tpl/${nom}.cbp.in
		${CMAKE_CURRENT_SOURCE_DIR}/${nom}.cbp
	)
	
	# Fini !
	message(STATUS "Préparation fichier cbp ${target} - done")
endfunction()

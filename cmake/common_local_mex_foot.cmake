# build unstable targets only with Unstable build types
if((NOT JVX_DEPLOY_UNSTABLE) AND IS_UNSTABLE)
  message("Excluding unstable target ${JVX_TARGET_NAME} from build.")
  return()
endif()

if(BUILD_MEX_OCTAVE AND BUILD_MEX_MATLAB)
  message(FATAL_ERROR "building matlab and octave mex files at once is not yet supported...")
endif()

# do not start mex files with lib...
set(CMAKE_SHARED_LIBRARY_PREFIX "")

if(IS_UNSTABLE)
  set(INSTALL_COMPONENT "unstable")
else()
  set(INSTALL_COMPONENT "release")
endif()

###
# Octave
###
if(BUILD_MEX_OCTAVE)

  message("    > MEX-File (octave) -- Targetname = ${JVX_TARGET_NAME}")

  # include_directories(${OCTAVE_INCLUDE_PATH})
  # link_directories(${OCTAVE_PATH_LIB})
  add_library(${JVX_TARGET_NAME} SHARED ${LOCAL_SOURCES})
  target_include_directories(${JVX_TARGET_NAME} PUBLIC
                           $<BUILD_INTERFACE:${Octave_INCLUDE_DIRS}>
                           $<INSTALL_INTERFACE:${Octave_INCLUDE_DIRS}>)
						   
  target_compile_definitions(${JVX_TARGET_NAME} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${GLOBAL_COMPILE_DEFINITIONS})
  set_target_properties(${JVX_TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "${JVX_CXX_FLAGS_MEX_OCTAVE} ${JVX_CMAKE_CXX_FLAGS_SHARED} ${LOCAL_CXX_FLAGS}"
    LINK_FLAGS "${JVX_CMAKE_LINKER_FLAGS_OCTAVE_MEX} ${LOCAL_LINKER_FLAGS}"
    SUFFIX "${OCTAVE_MEX_SUFFIX}")
  target_link_libraries(${JVX_TARGET_NAME} ${LOCAL_LIBS} ${LOCAL_OCT_LIBS} ${JVX_SYSTEM_OCTAVE_MEX_LIBRARIES} ${JVX_SYSTEM_LIBRARIES})
  if(JVX_LANGUAGE_SPECIFIC_OPTIONS)
		target_compile_options(${JVX_TARGET_NAME} PRIVATE ${JVX_LANGUAGE_SPECIFIC_OPTIONS})
	endif()
  jvx_displayFlags(${JVX_TARGET_NAME})

  # installation with a different name. This causes problems with Ninja since we will
  # see a duplicate target if Octave and Matlab are active
  #if(INSTALL_COMMON_MEX_TARGET_NAME)
  #  set_target_properties(${JVX_TARGET_NAME} PROPERTIES OUTPUT_NAME ${INSTALL_COMMON_MEX_TARGET_NAME})
  #endif()
  
  # Note that the RENAME option does not work in cmake currently 
  # install(TARGETS ${JVX_TARGET_NAME}
	#	RUNTIME DESTINATION ${INSTALL_PATH_OCTAVE}
    #   LIBRARY DESTINATION ${INSTALL_PATH_OCTAVE}
	#   COMPONENT ${INSTALL_COMPONENT}
	#   RENAME ${JVX_RENAME_RELEASE})
	
	install(TARGETS ${JVX_TARGET_NAME}
		RUNTIME DESTINATION ${INSTALL_PATH_OCTAVE}
        LIBRARY DESTINATION ${INSTALL_PATH_OCTAVE}
		COMPONENT ${INSTALL_COMPONENT})  
  
	if(JVX_RENAME_RELEASE)
		INSTALL(CODE "execute_process( \
			COMMAND \"${CMAKE_COMMAND}\" -E rename \
			${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_OCTAVE}/${JVX_TARGET_NAME}${OCTAVE_MEX_SUFFIX} \
			${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_OCTAVE}/${JVX_RENAME_RELEASE}${OCTAVE_MEX_SUFFIX}   \
			)")
	endif()
  
    if(JVX_CREATE_LOCAL_LINK)
		INSTALL(CODE "execute_process( \
			COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \
			${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_OCTAVE}/${JVX_TARGET_NAME} \
			${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_OCTAVE}/${JVX_CREATE_LOCAL_LINK}${OCTAVE_MEX_SUFFIX}   \
			)")
	endif() 
  
  FOREACH(LOCAL_DIR_ENTRY ${LOCAL_M_FILES_SOURCE_DIR})
	install(DIRECTORY ${LOCAL_DIR_ENTRY}/ DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT ${INSTALL_COMPONENT})
  ENDFOREACH()

  if(LOCAL_IMAGES_SOURCE_DIR)
    install(DIRECTORY ${LOCAL_IMAGES_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT})
  endif()
  if(LOCAL_START_SCRIPT_OCTAVE)
		message("Configuring file ${LOCAL_START_SCRIPT_OCTAVE}${JVX_SCRIPT_EXTENSION}.in for Octave start script")
		if(LOCAL_START_SCRIPT_OCTAVE_SCRIPT_NAME)
			configure_file(${LOCAL_START_SCRIPT_OCTAVE}${JVX_SCRIPT_EXTENSION}.in 
				${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_OCTAVE_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION})
			install( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_OCTAVE_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
		else()
			configure_file(${LOCAL_START_SCRIPT_OCTAVE}${JVX_SCRIPT_EXTENSION}.in 
				${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
			install( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
		endif()
  else()
    if(CONFIGURE_LOCAL_START_SCRIPT)
		set(JVX_OCTAVE_EXECUTABLE_REPLACE ${OCTAVE})
		message("Producing file ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION}")
		configure_file(${JVX_BASE_ROOT}/software/templates/scripts/startOctave${JVX_SCRIPT_EXTENSION}.${JVX_OS}.in ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
    endif()
  endif()

  	###
	# Some project folder groupings
	###
	if(JVX_MEX_FOLDER_HIERARCHIE)
		set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_MEX_FOLDER_HIERARCHIE}/mex-octave")
	else()
		if(JVX_MEX_FOLDER_HIERARCHIE_DEFAULT)
			set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_MEX_FOLDER_HIERARCHIE_DEFAULT}/mex-octave")
		else()
			set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/mex-octave")
		endif()
	endif()
			
	if(LOCAL_IMAGES_OCTAVE_SOURCE_DIR)
		install(DIRECTORY ${LOCAL_IMAGES_OCTAVE_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.png")
	endif()
endif()

###
# Matlab
###
if(BUILD_MEX_MATLAB)

  message("    > MEX-File (matlab)  -- Targetname = ${JVX_TARGET_NAME}")

  # include_directories(${Matlab_INCLUDE_DIRS})
  # link_directories(${MATLAB_PATH_LIB})
  add_library(${JVX_TARGET_NAME} SHARED ${LOCAL_SOURCES})
  target_include_directories(${JVX_TARGET_NAME} PUBLIC
                           $<BUILD_INTERFACE:${Matlab_INCLUDE_DIRS}>
                           $<INSTALL_INTERFACE:${Matlab_INCLUDE_DIRS}>)
						   
  if(BUILD_MEX_MATLAB_SIMULINK)
	target_include_directories(${JVX_TARGET_NAME} PUBLIC
							$<BUILD_INTERFACE:${Simulink_INCLUDE_DIRS}>
							$<INSTALL_INTERFACE:${Simulink_INCLUDE_DIRS}>)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};MATLAB_MEX_FILE")
  endif()
  
  target_compile_definitions(${JVX_TARGET_NAME} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${GLOBAL_COMPILE_DEFINITIONS})

  set_target_properties(${JVX_TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "${JVX_CXX_FLAGS_MEX_MATLAB} ${JVX_CMAKE_CXX_FLAGS_SHARED} ${LOCAL_CXX_FLAGS}"
    LINK_FLAGS "${JVX_CMAKE_LINKER_FLAGS_MATLAB_MEX} ${LOCAL_LINKER_FLAGS}"
    SUFFIX "${MATLAB_MEX_SUFFIX}")
  target_link_libraries(${JVX_TARGET_NAME} ${LOCAL_LIBS} ${LOCAL_MAT_LIBS} ${JVX_SYSTEM_MATLAB_MEX_LIBRARIES} ${JVX_SYSTEM_LIBRARIES})
  if(JVX_LANGUAGE_SPECIFIC_OPTIONS)
		target_compile_options(${JVX_TARGET_NAME} PRIVATE ${JVX_LANGUAGE_SPECIFIC_OPTIONS})
	endif()
  jvx_displayFlags(${JVX_TARGET_NAME})

  # installation with a different name. This causes problems with Ninja since we will
  # see a duplicate target if Octave and Matlab are active
  #if(INSTALL_COMMON_MEX_TARGET_NAME)
  #  set_target_properties(${JVX_TARGET_NAME} PROPERTIES OUTPUT_NAME ${INSTALL_COMMON_MEX_TARGET_NAME})
  #endif()
  
  # Note that the RENAME option does not work in cmake currently 
  #install(TARGETS ${JVX_TARGET_NAME}
	#	RUNTIME DESTINATION ${INSTALL_PATH_MATLAB}
    #    LIBRARY DESTINATION ${INSTALL_PATH_MATLAB}
	#	COMPONENT ${INSTALL_COMPONENT}
	#	RENAME ${JVX_RENAME_RELEASE})  
  install(TARGETS ${JVX_TARGET_NAME}
	RUNTIME DESTINATION ${INSTALL_PATH_MATLAB}
	LIBRARY DESTINATION ${INSTALL_PATH_MATLAB}
	COMPONENT ${INSTALL_COMPONENT})
	
	if(JVX_RENAME_RELEASE)
		if(DEDICATED_INSTALL_PATH_MATLAB)
		
			# Manual rename - cmake fails to rename the file possibly due to the target location outside the folder tree
			INSTALL(CODE "execute_process( \
				COMMAND \"${CMAKE_COMMAND}\" -E copy \
				${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_TARGET_NAME}${MATLAB_MEX_SUFFIX} \
				${DEDICATED_INSTALL_PATH_MATLAB}/${JVX_RENAME_RELEASE}${MATLAB_MEX_SUFFIX}   \
				)")
				
			# Instead of removing, we leave the output in the target folder with leading "__"
			#INSTALL(CODE "execute_process( \
			#	COMMAND \"${CMAKE_COMMAND}\" -E remove \
			#	${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_TARGET_NAME}${MATLAB_MEX_SUFFIX} \
			#	)")
				
			INSTALL(CODE "execute_process( \
				COMMAND \"${CMAKE_COMMAND}\" -E rename \
				${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_TARGET_NAME}${MATLAB_MEX_SUFFIX} \
				${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/__${JVX_RENAME_RELEASE}${MATLAB_MEX_SUFFIX}   \
				)")
				
		else()

			INSTALL(CODE "execute_process( \
				COMMAND \"${CMAKE_COMMAND}\" -E rename \
				${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_TARGET_NAME}${MATLAB_MEX_SUFFIX} \
				${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_RENAME_RELEASE}${MATLAB_MEX_SUFFIX}   \
				)")
		endif()
	endif()
	
    if(JVX_CREATE_LOCAL_LINK)
	
		INSTALL(CODE "execute_process( \
			COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \
			${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_TARGET_NAME} \
			${CMAKE_INSTALL_PREFIX}/${INSTALL_PATH_MATLAB}/${JVX_CREATE_LOCAL_LINK}${MATLAB_MEX_SUFFIX}   \
			)")
	endif()
	
  FOREACH(LOCAL_DIR_ENTRY ${LOCAL_M_FILES_SOURCE_DIR})
	install(DIRECTORY ${LOCAL_DIR_ENTRY}/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT ${INSTALL_COMPONENT})
  ENDFOREACH()

  if(LOCAL_IMAGES_SOURCE_DIR)
	install(DIRECTORY ${LOCAL_IMAGES_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT})
  endif()
  if(LOCAL_START_SCRIPT_MATLAB)
		message("Configuring file ${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in for Matlab start script")
		if(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME)
			message("Intermediate file -> ${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION}")
			configure_file(${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in 
				${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION})
			install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION}  DESTINATION ${INSTALL_PATH_SCRIPTS})			
		else()
			configure_file(${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in 
				${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
			install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION}  DESTINATION ${INSTALL_PATH_SCRIPTS})
		endif()
		
  else()
	if(CONFIGURE_LOCAL_START_SCRIPT)
		set(JVX_MATLAB_EXECUTABLE_REPLACE ${Matlab_MAIN_PROGRAM})
		message("Producing file ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION}")
		configure_file(${JVX_BASE_ROOT}/software/templates/scripts/startMatlab${JVX_SCRIPT_EXTENSION}.${JVX_OS}.in ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
	endif()
  endif()

  	###
	# Some project folder groupings
	###
	if(BUILD_MEX_MATLAB_SIMULINK)
	
		if(JVX_MEX_SIMULINK_FOLDER_HIERARCHIE)
			set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_MEX_SIMULINK_FOLDER_HIERARCHIE}/mex-matlab-simulink")
		else()
			if(JVX_MEX_SIMULINK_FOLDER_HIERARCHIE_DEFAULT)
				set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_MEX_SIMULINK_FOLDER_HIERARCHIE_DEFAULT}/mex-matlab-simulink")
			else()
				set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/mex-matlab-simulink")
			endif()
		endif()
	
		message("-> ${JVX_MEX_SIMULINK_FOLDER_HIERARCHIE} -- ${JVX_FOLDER_HIERARCHIE}")
	else()
		
		if(JVX_MEX_FOLDER_HIERARCHIE)
			set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_MEX_FOLDER_HIERARCHIE}/mex-matlab")
		else()
			if(JVX_MEX_FOLDER_HIERARCHIE_DEFAULT)
				set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_MEX_FOLDER_HIERARCHIE_DEFAULT}/mex-matlab")
			else()
				set_property(TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/mex-matlab")
			endif()
		endif()
		
		message("-> ${JVX_MEX_FOLDER_HIERARCHIE} -- ${JVX_FOLDER_HIERARCHIE} -- ${lvar}")
	endif()
	
	get_property(lvar TARGET ${JVX_TARGET_NAME} PROPERTY FOLDER)
	message(" --> FOLDER property in target <${JVX_TARGET_NAME}> => ${lvar}")
	
	if(LOCAL_IMAGES_MATLAB_SOURCE_DIR)
		message("Local image copy: Folder ${LOCAL_IMAGES_MATLAB_SOURCE_DIR} to ${INSTALL_COMPONENT}, file pattern *.png")
		install(DIRECTORY ${LOCAL_IMAGES_MATLAB_SOURCE_DIR}/ DESTINATION ${INSTALL_PATH_IMAGES} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.png")
	endif()
endif()

if(NOT EXISTS JVX_COLLECT_INSTALL_DIRECTORIES)
	set(JVX_COLLECT_INSTALL_DIRECTORIES FALSE)
endif()
jvx_install_project_folders(${JVX_COLLECT_INSTALL_DIRECTORIES})

###
# error
###
if(NOT BUILD_MEX_OCTAVE AND NOT BUILD_MEX_MATLAB)
  message(FATAL_ERROR "Library module must define BUILD_MEX_OCTAVE or BUILD_MEX_MATLAB")
endif()


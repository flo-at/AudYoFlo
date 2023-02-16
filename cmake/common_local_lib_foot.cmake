# build unstable targets only with Unstable build types
if((NOT JVX_DEPLOY_UNSTABLE) AND IS_UNSTABLE)
  message("Excluding unstable target ${JVX_TARGET_NAME} from build.")
  return()
endif()

###
# reset some stuff
###
set(ADDITIONAL_CXX_FLAGS "")
set(ADDITIONAL_LIBS "")
set(ADDITIONAL_SOURCES "")
set(ADDITIONAL_COMPILE_DEFINITIONS "")
set(ADDITIONAL_LINKER_FLAGS "")
set(GENERATED_FILES "")

if(IS_UNSTABLE)
  set(INSTALL_COMPONENT "unstable")
else()
  set(INSTALL_COMPONENT "release")
endif()


###
# JVX Components are always built static AND shared
###
if(IS_JVX_COMPONENT)

  if(JVX_COMPONENT_ONLY_SHARED)
    set(BUILD_SHARED TRUE)
	set(BUILD_STATIC FALSE)
  else()
    set(BUILD_STATIC TRUE)
	if(JVX_COMPONENT_ONLY_STATIC)
		set(BUILD_SHARED FALSE)
	else(JVX_COMPONENT_ONLY_STATIC)
		if(NOT JVX_DISABLE_ALL_SHARED)
			set(BUILD_SHARED TRUE)
		else()
			set(BUILD_SHARED FALSE)
		endif()
	endif(JVX_COMPONENT_ONLY_STATIC)
  endif()
endif()

###
# run PCG
###
if(LOCAL_PCG_FILES)
  message("    > PCG (${JVX_PCG})")
  set(PCG_OUT_PREFIX "pcg_")
  if(JVX_PCG_LOCAL_PREFIX)
	set(PCG_OUT_PREFIX ${JVX_PCG_LOCAL_PREFIX})
  endif()
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
  set(ADDITIONAL_PCG_FILE_DEPENDS_LST "")
  foreach(PCGFILE ${LOCAL_PCG_FILES})
    get_filename_component(PCGDIR ${PCGFILE} DIRECTORY)
    get_filename_component(PCGTOKEN ${PCGFILE} NAME_WE)
    message("      ${PCGTOKEN}")
    execute_process(COMMAND ${JVX_PCG} -M ${PCGFILE} ${LOCAL_PCG_FILE_OPTIONS} OUTPUT_VARIABLE ADDITIONAL_PCG_FILE_DEPENDS)

    if(ADDITIONAL_PCG_FILE_DEPENDS)

      foreach(PCGDEPFILE ${ADDITIONAL_PCG_FILE_DEPENDS})

	# On windows, remove backslashes
	string(REGEX REPLACE "\\\\" "/" PCGDEPFILE_ADD ${PCGDEPFILE})
	set(ADDITIONAL_PCG_FILE_DEPENDS_LST ${ADDITIONAL_PCG_FILE_DEPENDS_LST} ${PCGDEPFILE_ADD})
      endforeach()
      message("      depends on: ${ADDITIONAL_PCG_FILE_DEPENDS} ${LOCAL_PCG_FILE_DEPENDS}")
    endif()
    add_custom_command(
      COMMAND ${JVX_PCG}
      ARGS ${PCGDIR}/${PCGTOKEN}.pcg -o ${CMAKE_CURRENT_BINARY_DIR}/generated/${PCG_OUT_PREFIX}${PCGTOKEN}.h ${LOCAL_PCG_FILE_OPTIONS}
      DEPENDS ${PCGDIR}/${PCGTOKEN}.pcg ${JVX_PCG} ${LOCAL_PCG_FILE_DEPENDS} ${ADDITIONAL_PCG_FILE_DEPENDS_LST}
	  # BYPRODUCTS byproduct_${PCGTOKEN}.txt # explicit byproduct specification
      OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/generated/${PCG_OUT_PREFIX}${PCGTOKEN}.h
	  )
	#message("-PCG-> Producing byproduct file byproduct_${PCGTOKEN}.txt")
    set(GENERATED_FILES ${GENERATED_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/${PCG_OUT_PREFIX}${PCGTOKEN}.h)
  endforeach()
 
  set(PCG_SOURCES  ${ADDITIONAL_PCG_FILE_DEPENDS_LST})
  message("      Build Options: ${LOCAL_PCG_FILE_OPTIONS}")
endif()

###
# run MCG
###
if(LOCAL_MCG_FILES)
  message("    > MCG (${JVX_MCG})")
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
  foreach(MCGFILE ${LOCAL_MCG_FILES})
    get_filename_component(MCGDIR ${MCGFILE} DIRECTORY)
    get_filename_component(MCGTOKEN ${MCGFILE} NAME_WE)
    message("      ${MCGTOKEN}")
    add_custom_command(
      COMMAND ${JVX_MCG}
      ARGS ${MCGDIR}/${MCGTOKEN}.mcg --out-h ${CMAKE_CURRENT_BINARY_DIR}/generated/mcg_${MCGTOKEN}.h --out-hp ${CMAKE_CURRENT_BINARY_DIR}/generated/mcg_${MCGTOKEN}_prototypes.h
      DEPENDS ${MCGDIR}/${MCGTOKEN}.mcg ${JVX_MCG}
	  #BYPRODUCTS byproduct_${MCGTOKEN}.txt # explicit byproduct specification
	  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/generated/mcg_${MCGTOKEN}.h ${CMAKE_CURRENT_BINARY_DIR}/generated/mcg_${MCGTOKEN}_prototypes.h
      )
    set(GENERATED_FILES ${GENERATED_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/mcg_${MCGTOKEN}.h)
    set(GENERATED_FILES ${GENERATED_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/mcg_${MCGTOKEN}_prototypes.h)
  endforeach()
endif()

###
# QT support
###
if(LOCAL_UI_SOURCES OR ADDITIONAL_UI_SOURCES OR USE_QT_LIBRARIES OR LOCAL_QT_RESOURCES OR LOCAL_UI_SOURCE_PATH)
	message("    > Executable (QT)")
	cmake_policy(SET CMP0020 NEW)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC_SEARCH_PATHS ${LOCAL_UI_SOURCE_PATH})

  #QT5_WRAP_UI(UI_HEADERS ${LOCAL_UI_SOURCES} ${ADDITIONAL_UI_SOURCES})
  #qt5_add_resources(QT_RESOURCES ${LOCAL_QT_RESOURCES})
  set(QT_RESOURCES ${LOCAL_QT_RESOURCES})
  
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ${JVX_QT_LIBS})
else()
  message("    > Executable")
endif()

# If Matlab scripts shall be installed, do that here
if(NOT JVX_RELEASE_SDK)
  FOREACH(LOCAL_DIR_ENTRY ${LOCAL_M_FILES_SOURCE_DIR})
    install(DIRECTORY ${LOCAL_DIR_ENTRY}/ DESTINATION ${INSTALL_PATH_MATLAB}/m-files COMPONENT ${INSTALL_COMPONENT})
    install(DIRECTORY ${LOCAL_DIR_ENTRY}/ DESTINATION ${INSTALL_PATH_OCTAVE}/m-files COMPONENT ${INSTALL_COMPONENT})
  ENDFOREACH()
endif()

###
# run octave
###
if (LOCAL_OCTCONF_SCRIPTS OR USES_OTHER_LOCAL_OCTCONF_SCRIPTS)
  if(JVX_USE_PART_OCTAVE)
    message("    > Octave (config header generation)")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
    foreach(LOCAL_OCTCONF_SCRIPT ${LOCAL_OCTCONF_SCRIPTS})
      message("execute_process(COMMAND ${OCTAVE} -qf --no-gui ${LOCAL_OCTCONF_SCRIPT} show_filename OUTPUT_VARIABLE OUTFILE)")
      execute_process(COMMAND ${OCTAVE} -qf --no-gui ${LOCAL_OCTCONF_SCRIPT} show_filename OUTPUT_VARIABLE OUTFILE)
      string(REPLACE "\n" ";" OUTFILE ${OUTFILE})
      JVX_PREPEND(OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/generated/" ${OUTFILE})
      message("      ${LOCAL_OCTCONF_SCRIPT} -> ${OUTFILE}")
      add_custom_command(
        COMMAND ${OCTAVE}
        ARGS -qf ${LOCAL_OCTCONF_SCRIPT} ${OUTFILE} ${JVX_BASE_LIBS_MATOCT_GEN_LIB} ${LOCAL_OCTCONF_ADDITIONAL_PATHS}
        OUTPUT  ${OUTFILE}
        DEPENDS ${LOCAL_OCTCONF_SCRIPT} ${OCTAVE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Running ${OCTAVE} -qf ${LOCAL_OCTCONF_SCRIPT} ${OUTFILE} ${JVX_BASE_LIBS_MATOCT_GEN_LIB} ${LOCAL_OCTCONF_ADDITIONAL_PATHS}")
      set(GENERATED_FILES ${GENERATED_FILES} ${OUTFILE})
    endforeach()
  else()
    message("    > Octave not available (use pre-generated config header)")
    set(ADDITIONAL_COMPILE_DEFINITIONS "${ADDITIONAL_COMPILE_DEFINITIONS};JVX_DSP_USE_DEFAULT_OCTCONF")
  endif()
endif()

if (LOCAL_MATCONF_SCRIPTS OR USES_OTHER_LOCAL_MATCONF_SCRIPTS)
  if(JVX_USE_PART_MATLAB)
    message("    > Matlab (config header generation)")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
    foreach(LOCAL_MATCONF_SCRIPT ${LOCAL_MATCONF_SCRIPTS})

      if(JVX_OS MATCHES "windows" OR JVX_OS MATCHES "mingw")
	set(MATLAB_START_FLAGS -wait -nodisplay -nosplash -nodesktop -nojvm)
      else()
	set(MATLAB_START_FLAGS "-wait -nodisplay -nosplash -nodesktop -nojvm") #-nodisplay -nosplash -nodesktop -nojvm
      endif()
      message("execute_process(COMMAND ${Matlab_MAIN_PROGRAM} ${MATLAB_START_FLAGS} -r \"global arg_list; arg_list = {'show_filename'}; global out_file; out_file = '${CMAKE_CURRENT_BINARY_DIR}/generated/myMatLog.log'; run('${LOCAL_MATCONF_SCRIPT}'); ${JVX_PRE_EXIT_MATLAB_HOOK} exit;\" OUTPUT_VARIABLE OUTFILE)")
      execute_process(COMMAND ${Matlab_MAIN_PROGRAM} ${MATLAB_START_FLAGS} -r "global arg_list; arg_list = {'show_filename'}; global out_file; out_file = '${CMAKE_CURRENT_BINARY_DIR}/generated/myMatLog.log'; run('${LOCAL_MATCONF_SCRIPT}'); ${JVX_PRE_EXIT_MATLAB_HOOK} exit;" OUTPUT_VARIABLE OUTFILE)
      if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/generated/myMatLog.log")
           file(READ "${CMAKE_CURRENT_BINARY_DIR}/generated/myMatLog.log" OUTFILE)
      else()
           set(OUTFILE "matlogoutfile_tmp")
      endif()
      string(REPLACE "\n" ";" OUTFILE ${OUTFILE})
      JVX_PREPEND(OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/generated/" ${OUTFILE})
      message("      ${LOCAL_MATCONF_SCRIPT} -> ${OUTFILE}")
      set(__MATLAB_TMP_LIST "")
      set(__TMP_LIST_ENTRIES ${OUTFILE} ${JVX_BASE_LIBS_MATOCT_GEN_LIB} ${LOCAL_MATCONF_ADDITIONAL_PATHS})
      foreach(__TMP_NAME ${__TMP_LIST_ENTRIES})
	set(__MATLAB_TMP_LIST ${__MATLAB_TMP_LIST} '${__TMP_NAME}')
      endforeach()
      add_custom_command(
        COMMAND ${Matlab_MAIN_PROGRAM}
        ARGS ${MATLAB_START_FLAGS} -r \"global arg_list\; arg_list = { ${__MATLAB_TMP_LIST} }\; run('${LOCAL_MATCONF_SCRIPT}')\; exit\;\"
        OUTPUT  ${OUTFILE}
        DEPENDS ${LOCAL_MATCONF_SCRIPT} ${Matlab_MAIN_PROGRAM}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
      set(GENERATED_FILES ${GENERATED_FILES} ${OUTFILE})
    endforeach()
  else()
    message("    > Matlab not available (use pre-generated config header)")
    set(ADDITIONAL_COMPILE_DEFINITIONS "${ADDITIONAL_COMPILE_DEFINITIONS};JVX_DSP_USE_DEFAULT_MATCONF")
  endif()
endif()

###
# static lib
###
if(BUILD_STATIC)

  if(CUDA_SOURCES)
    add_library(${JVX_TARGET_NAME_STATIC} STATIC ${LOCAL_SOURCES} ${CUDA_SOURCES} ${ADDITIONAL_SOURCES} ${GENERATED_FILES} ${LOCAL_PCG_FILES} ${PCG_SOURCES} ${LOCAL_MCG_FILES} ${QT_RESOURCES} ${LOCAL_UI_SOURCES})

  else()
    add_library(${JVX_TARGET_NAME_STATIC} STATIC ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES} ${GENERATED_FILES} ${LOCAL_PCG_FILES} ${PCG_SOURCES} ${LOCAL_MCG_FILES} ${QT_RESOURCES} ${LOCAL_UI_SOURCES})
  endif()

  # Copy variable used by both BUILD_STATIC and BUILD_SHARED but modified in this block
  set(ADDITIONAL_COMPILE_DEFINITIONS_STATIC "${ADDITIONAL_COMPILE_DEFINITIONS}")

  # The cmake code generator for ios is somewhat buggy. In order to be able to debug libs compiled with
  # xcodebuild, the following was told to allow debugging
  if(IOS)
    set_xcode_property(${JVX_TARGET_NAME_STATIC} GCC_GENERATE_DEBUGGING_SYMBOLS YES "all")
  endif()

  # Javox component libs get different compile definitions and a generated header
  if(IS_JVX_COMPONENT)
    message("    > Javox component library (static)")
    string(REPLACE "-" "_" JVX_TARGET_NAME_GEN_STATIC ${JVX_TARGET_NAME})
    set(ADDITIONAL_COMPILE_DEFINITIONS_STATIC "${ADDITIONAL_COMPILE_DEFINITIONS_STATIC};JVX_OBJECT_INIT_FUNCTION=${JVX_TARGET_NAME_GEN_STATIC}_init;JVX_OBJECT_TERMINATE_FUNCTION=${JVX_TARGET_NAME_GEN_STATIC}_terminate;JVX_MODULE_NAME=${JVX_MODULE_NAME};JVX_COMPONENT_ACCESS_TYPE_COMPONENT=JVX_COMPONENT_ACCESS_STATIC_LIB;JVX_IMPORT_DECLARATION=/* no export*/")
    configure_file(${JVX_BASE_ROOT}/software/templates/objectAccess.h.in ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.h)
	## add_library(${JVX_TARGET_NAME_STATIC} STATIC ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.h)
	if(JVX_PROPERTY_ACCESS_HEADER_COMPONENT)
		configure_file(${JVX_PROPERTY_ACCESS_HEADER_COMPONENT} ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}_props.h)
	endif()
	if(JVX_INTERFACE_ACCESS_HEADER_COMPONENT)
		configure_file(${JVX_INTERFACE_ACCESS_HEADER_COMPONENT} ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}_if.h)
	endif()
  else()
    message("    > Library (static)")
  endif()

	if(JVX_AUTOGEN_REPLACEMENT)
		## Resources:
		## https://stackoverflow.com/questions/60920473/cant-include-ui-file-from-linked-library-using-cmakes-autouic
		message("    > Applying replacement Autogen folder definition ${JVX_AUTOGEN_REPLACEMENT}, hence, all generated files will be in ${CMAKE_CURRENT_BINARY_DIR}/${JVX_AUTOGEN_REPLACEMENT}")
		set_target_properties(${JVX_TARGET_NAME_STATIC} PROPERTIES AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${JVX_AUTOGEN_REPLACEMENT}")
	endif()
	
  # set flags
  target_compile_definitions(${JVX_TARGET_NAME_STATIC} PRIVATE ${LOCAL_COMPILE_DEFINITIONS} ${ADDITIONAL_COMPILE_DEFINITIONS_STATIC}  ${LOCAL_COMPILE_DEFINITIONS_STATIC} ${GLOBAL_COMPILE_DEFINITIONS})
  if(IS_C_LIB)
    set_target_properties(${JVX_TARGET_NAME_STATIC} PROPERTIES

      message("##### JVX_CMAKE_C_FLAGS_SHARED=${JVX_CMAKE_C_FLAGS_SHARED}")
	  message("##### LOCAL_C_FLAGS=${LOCAL_C_FLAGS}")
	  message("##### ADDITIONAL_C_FLAGS=${ADDITIONAL_C_FLAGS}")
	  message("##### JVX_LANGUAGE_SPECIFIC_OPTIONS=${JVX_LANGUAGE_SPECIFIC_OPTIONS}")
	
      COMPILE_FLAGS "${JVX_CMAKE_C_FLAGS_STATIC_PIC} ${LOCAL_C_FLAGS} ${ADDITIONAL_C_FLAGS}"
      LINK_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_STATIC_PIC} ${LOCAL_LINKER_FLAGS} ${ADDITIONAL_LINKER_FLAGS}")

    ###
    # Some project folder groupings
    ###
    set_property(TARGET ${JVX_TARGET_NAME_STATIC} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-static-c")
  else()

    message("##### JVX_CMAKE_CXX_FLAGS_SHARED=${JVX_CMAKE_CXX_FLAGS_SHARED}")
    message("##### LOCAL_CXX_FLAGS=${LOCAL_CXX_FLAGS}")
    message("##### ADDITIONAL_CXX_FLAGS=${ADDITIONAL_CXX_FLAGS}")
	message("##### JVX_LANGUAGE_SPECIFIC_OPTIONS=${JVX_LANGUAGE_SPECIFIC_OPTIONS}")
	
    set_target_properties(${JVX_TARGET_NAME_STATIC} PROPERTIES
      COMPILE_FLAGS "${JVX_CMAKE_CXX_FLAGS_STATIC_PIC} ${LOCAL_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}"
      LINK_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_STATIC_PIC} ${LOCAL_LINKER_FLAGS} ${ADDITIONAL_LINKER_FLAGS}")

    ###
    # Some project folder groupings
    ###
    set_property(TARGET ${JVX_TARGET_NAME_STATIC} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-static-cpp")
  endif()
  set(CUDA_LIBS "")
    if(CUDA_SOURCES)
      if(CUDA_USE_FFTW)
		set(CUDA_LIBS "${CUDA_LIBS};cufft")
      endif(CUDA_USE_FFTW)
    endif()
  target_link_libraries(${JVX_TARGET_NAME_STATIC} ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES} ${CUDA_LIBS})
	
  if(JVX_LANGUAGE_SPECIFIC_OPTIONS)
	target_compile_options(${JVX_TARGET_NAME_STATIC} PRIVATE ${JVX_LANGUAGE_SPECIFIC_OPTIONS})
  endif()
			
  jvx_displayFlags(${JVX_TARGET_NAME_STATIC})

  # installation
  if(IS_JVX_COMPONENT)

    if(JVX_RELEASE_SDK OR JVX_USES_SDK)
		if(NOT JVX_NO_WRITE_SDK)
			# Here, we need to define a special rule for ios since there is a 5 year old (!) bug in cmake
			# There was someone working on a fix in 2015 but this seems not yet integrated into my cmake version
			if (IOS)
				install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}-${SDK_NAME}/${JVX_STATIC_PREFIX}${JVX_TARGET_NAME_STATIC}.${JVX_STATIC_EXTENSION} DESTINATION ${INSTALL_PATH_COMPONENTS_STATIC})
			else()
				install(TARGETS ${JVX_TARGET_NAME_STATIC} DESTINATION ${INSTALL_PATH_COMPONENTS_STATIC} COMPONENT ${INSTALL_COMPONENT})
			endif()

			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.h DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_STATIC} COMPONENT ${INSTALL_COMPONENT})
			install(FILES ${LOCAL_ADDITIONAL_INSTALL_HEADERS} DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_STATIC} COMPONENT ${INSTALL_COMPONENT})
		else()
			message("	> ! SDK not writeable, skipping lib install step. !")
		endif()
    endif()
  else()

    if(JVX_RELEASE_SDK OR JVX_USES_SDK)
		if(NOT JVX_NO_WRITE_SDK)

			get_filename_component(suffix ${CMAKE_CURRENT_SOURCE_DIR}  NAME )

			# Here, we need to define a special rule for ios since there is a 5 year old (!) bug in cmake
			# There was someone working on a fix in 2015 but this seems not yet integrated into my cmake version
			if (IOS)
				install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}-${SDK_NAME}/${JVX_STATIC_PREFIX}${JVX_TARGET_NAME_STATIC}.${JVX_STATIC_EXTENSION} DESTINATION ${INSTALL_PATH_LIB_STATIC})
			else()
				install(TARGETS ${JVX_TARGET_NAME_STATIC} DESTINATION ${INSTALL_PATH_LIB_STATIC} COMPONENT ${INSTALL_COMPONENT})
			endif()

			if(INSTALL_DEDICATED_INCLUDE_TREE)
				install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
			endif()

			if(INSTALL_DEDICATED_GENERATED_TREE)
				install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
			endif()

			if(INSTALL_DEDICATED_PCG_TREE)
				install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pcg DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix} COMPONENT ${INSTALL_COMPONENT})
			endif()

			if(LOCAL_ADDITIONAL_INSTALL_PATH)
				install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${LOCAL_ADDITIONAL_INSTALL_PATH}/ DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix}/${LOCAL_ADDITIONAL_INSTALL_PATH} COMPONENT ${INSTALL_COMPONENT} FILES_MATCHING PATTERN "*.h")
			endif()

			if(LOCAL_ADDITIONAL_INSTALL_HEADERS)
				install(FILES ${LOCAL_ADDITIONAL_INSTALL_HEADERS} DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix}/include COMPONENT ${INSTALL_COMPONENT})
			endif()

			if(GENERATED_ADDITIONAL_INSTALL_HEADERS)
				# message(FATAL_ERROR "-->${GENERATED_ADDITIONAL_INSTALL_HEADERS}")
				install(FILES ${GENERATED_ADDITIONAL_INSTALL_HEADERS} DESTINATION ${INSTALL_PATH_INCLUDE_STATIC}/${suffix}/generated COMPONENT ${INSTALL_COMPONENT})
			endif()
			
		else()
			message("	> ! SDK not writeable, skipping lib install step. !")
		endif()
    endif()
  endif()

endif()


###
# shared lib
###
# Build the shared objects only if in runtime relase mode
if(BUILD_SHARED)

  if(NOT JVX_RELEASE_SDK OR (JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB AND NOT JVX_MATLAB_PROJECT_CLONE))
	
	if(JVX_FORCE_RUNTIME_IMPORTLIB)
		set(LOCAL_TARGET_NAME_SHARED ${JVX_TARGET_NAME}_import)		
	else()
		set(LOCAL_TARGET_NAME_SHARED ${JVX_TARGET_NAME})
	endif()
		
    set(ADDITIONAL_COMPILE_DEFINITIONS_SHARED "${ADDITIONAL_COMPILE_DEFINITIONS}")

    # Javox component libs get different linker flags
    if(IS_JVX_COMPONENT)
		if(JVX_RELEASE_SDK AND JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB)  
			message("    > Javox component library (static)")
			string(REPLACE "-" "_" JVX_TARGET_NAME_GEN_STATIC ${JVX_TARGET_NAME})
			set(ADDITIONAL_COMPILE_DEFINITIONS_SHARED "${ADDITIONAL_COMPILE_DEFINITIONS_STATIC};JVX_OBJECT_INIT_FUNCTION=${JVX_TARGET_NAME_GEN_STATIC}_init;JVX_OBJECT_TERMINATE_FUNCTION=${JVX_TARGET_NAME_GEN_STATIC}_terminate;JVX_MODULE_NAME=${JVX_MODULE_NAME};JVX_COMPONENT_ACCESS_TYPE_COMPONENT=JVX_COMPONENT_ACCESS_IMPORT_LIB;;JVX_IMPORT_DECLARATION=${JVX_SHARED_EXPORT}")
			configure_file(${JVX_BASE_ROOT}/software/templates/objectAccess.h.in ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.h)
			set(LOCAL_GENERATED_FILES "${LOCAL_GENERATED_FILES};${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.h")
			set(JVX_ALSO_INSTALL_IMPORT_LIB TRUE)
			set(LOCAL_TARGET_NAME_SHARED ${JVX_TARGET_NAME_IMPORT})
		else()
			message("    > Javox component library (shared)")
			set(ADDITIONAL_COMPILE_DEFINITIONS_SHARED "${ADDITIONAL_COMPILE_DEFINITIONS_SHARED};JVX_MODULE_NAME=${JVX_MODULE_NAME};;JVX_COMPONENT_ACCESS_TYPE_COMPONENT=JVX_COMPONENT_ACCESS_DYNAMIC_LIB;JVX_IMPORT_DECLARATION=/*export in module file*/")
			set(ADDITIONAL_LINKER_FLAGS "${ADDITIONAL_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_SHARED_EXPORT_COMPONENTS}")
			set(ADDITIONAL_SOURCES ${ADDITIONAL_SOURCES} ${JVX_EXPORTS_SHARED_COMPONENT})
      set(JVX_COLLECT_COMPONENTS_MAC_BUNDLE "${JVX_COLLECT_COMPONENTS_MAC_BUNDLE};${JVX_TARGET_NAME}.${JVX_SHARED_EXTENSION}" CACHE INTERNAL "all components")
		endif()	
		
		if(JVX_PROPERTY_ACCESS_HEADER_COMPONENT)
			configure_file(${JVX_PROPERTY_ACCESS_HEADER_COMPONENT} ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}_props.h)
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}_props.h DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_IMPORT} COMPONENT ${INSTALL_COMPONENT})
		endif()
		if(JVX_INTERFACE_ACCESS_HEADER_COMPONENT)
			configure_file(${JVX_INTERFACE_ACCESS_HEADER_COMPONENT} ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}_if.h)
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}_if.h DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_IMPORT} COMPONENT ${INSTALL_COMPONENT})
		endif()
    else()
		if(JVX_RELEASE_SDK AND JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB)  
			set(JVX_ALSO_INSTALL_IMPORT_LIB TRUE)
		endif()
		set(ADDITIONAL_COMPILE_DEFINITIONS_SHARED "JVX_IMPORT_DECLARATION=${JVX_SHARED_EXPORT}")      
		message("    > Library (shared)")
    endif()

	if(CUDA_SOURCES)
      
      add_library(${LOCAL_TARGET_NAME_SHARED} SHARED ${LOCAL_SOURCES} ${CUDA_SOURCES} ${ADDITIONAL_SOURCES} ${LOCAL_GENERATED_FILES} ${GENERATED_FILES}  ${LOCAL_PCG_FILES} ${PCG_SOURCES} ${LOCAL_MCG_FILES} ${QT_RESOURCES} ${UI_HEADERS})

    else()
      add_library(${LOCAL_TARGET_NAME_SHARED} SHARED ${LOCAL_SOURCES} ${ADDITIONAL_SOURCES} ${LOCAL_GENERATED_FILES} ${GENERATED_FILES}  ${LOCAL_PCG_FILES} ${PCG_SOURCES} ${LOCAL_MCG_FILES} ${QT_RESOURCES} ${UI_HEADERS})
    endif()
	

	if(JVX_OUTPUT_NAME_MODIFY)
		set_target_properties(${LOCAL_TARGET_NAME_SHARED} PROPERTIES OUTPUT_NAME "${JVX_OUTPUT_NAME_MODIFY}")
	endif()
	if(JVX_OUTPUT_NAME_SUFFIX)
		set_target_properties(${LOCAL_TARGET_NAME_SHARED} PROPERTIES SUFFIX "${JVX_OUTPUT_NAME_SUFFIX}")
	endif()

    # set flags
    target_compile_definitions(${LOCAL_TARGET_NAME_SHARED} PRIVATE "JVX_SHARED_LIB" ${LOCAL_COMPILE_DEFINITIONS} ${ADDITIONAL_COMPILE_DEFINITIONS_SHARED} ${LOCAL_COMPILE_DEFINITIONS_SHARED} ${GLOBAL_COMPILE_DEFINITIONS})
    if(IS_C_LIB)

      	message("##### JVX_CMAKE_C_FLAGS_SHARED=${JVX_CMAKE_C_FLAGS_SHARED}")
		message("##### LOCAL_C_FLAGS=${LOCAL_C_FLAGS}")
		message("##### ADDITIONAL_C_FLAGS=${ADDITIONAL_C_FLAGS}")
		message("##### JVX_LANGUAGE_SPECIFIC_OPTIONS=${JVX_LANGUAGE_SPECIFIC_OPTIONS}")
		
		set_target_properties(${LOCAL_TARGET_NAME_SHARED} PROPERTIES
		COMPILE_FLAGS "${CMAKE_C_FLAGS} ${JVX_CMAKE_C_FLAGS_SHARED} ${LOCAL_C_FLAGS} ${ADDITIONAL_C_FLAGS}"
		LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_SHARED} ${LOCAL_LINKER_FLAGS} ${ADDITIONAL_LINKER_FLAGS}")
		
		###
		# Some project folder groupings
		###
		if(JVX_RELEASE_SDK AND JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB)  
			set_property(TARGET ${LOCAL_TARGET_NAME_SHARED} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-import-c")
		else()
			set_property(TARGET ${LOCAL_TARGET_NAME_SHARED} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-dynamic-c")
		endif()

    else()
      set_target_properties(${LOCAL_TARGET_NAME_SHARED} PROPERTIES

		message("##### JVX_CMAKE_CXX_FLAGS_SHARED=${JVX_CMAKE_CXX_FLAGS_SHARED}")
		message("##### LOCAL_CXX_FLAGS=${LOCAL_CXX_FLAGS}")
		message("##### ADDITIONAL_CXX_FLAGS=${ADDITIONAL_CXX_FLAGS}")
		message("##### JVX_LANGUAGE_SPECIFIC_OPTIONS=${JVX_LANGUAGE_SPECIFIC_OPTIONS}")

		# COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${JVX_CMAKE_CXX_FLAGS_SHARED} ${LOCAL_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}"
		COMPILE_FLAGS "${JVX_CMAKE_CXX_FLAGS_SHARED} ${LOCAL_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}" 
		LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${JVX_CMAKE_LINKER_FLAGS_SHARED} ${LOCAL_LINKER_FLAGS} ${ADDITIONAL_LINKER_FLAGS}")

		###
		# Some project folder groupings
		###
		if(JVX_RELEASE_SDK AND JVX_SDK_FORCE_DEPLOY_IMPORT_STATIC_LIB)  
			set_property(TARGET ${LOCAL_TARGET_NAME_SHARED} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-import-cpp")
		else()
			set_property(TARGET ${LOCAL_TARGET_NAME_SHARED} PROPERTY FOLDER "${JVX_FOLDER_HIERARCHIE}/libraries-dynamic-cpp")
		endif()

    endif()

    set(CUDA_LIBS "")
    if(CUDA_SOURCES)
      if(CUDA_USE_FFTW)
		set(CUDA_LIBS "${CUDA_LIBS};cufft")
      endif(CUDA_USE_FFTW)
    endif()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(${LOCAL_TARGET_NAME_SHARED} ${JVX_SYSTEM_LIBRARIES} ${LOCAL_LIBS} ${ADDITIONAL_LIBS} ${JVX_SYSTEM_LIBRARIES} ${CUDA_LIBS})
	if(JVX_LANGUAGE_SPECIFIC_OPTIONS)
		target_compile_options(${LOCAL_TARGET_NAME_SHARED} PRIVATE ${JVX_LANGUAGE_SPECIFIC_OPTIONS})
	endif()
	
    jvx_displayFlags(${LOCAL_TARGET_NAME_SHARED})

    # installation
    if(IS_JVX_COMPONENT)
		if(JVX_ALSO_INSTALL_IMPORT_LIB)
			install(TARGETS ${LOCAL_TARGET_NAME_SHARED}
				RUNTIME DESTINATION ${INSTALL_PATH_COMPONENTS_SHARED}
				LIBRARY DESTINATION ${INSTALL_PATH_COMPONENTS_SHARED} 
				ARCHIVE DESTINATION ${INSTALL_PATH_COMPONENTS_IMPORT} COMPONENT ${INSTALL_COMPONENT})
			install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${JVX_TARGET_NAME}.h DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_IMPORT} COMPONENT ${INSTALL_COMPONENT})
			install(FILES ${LOCAL_ADDITIONAL_INSTALL_HEADERS} DESTINATION ${INSTALL_PATH_INCLUDE_COMPONENTS_STATIC} COMPONENT ${INSTALL_COMPONENT})
		else()
			install(TARGETS ${LOCAL_TARGET_NAME_SHARED}
				RUNTIME DESTINATION ${INSTALL_PATH_COMPONENTS_SHARED}
				LIBRARY DESTINATION ${INSTALL_PATH_COMPONENTS_SHARED} COMPONENT ${INSTALL_COMPONENT})
		endif()
    else()

		if(JVX_SHARED_LIB_TARGET_FOLDER)
			install(TARGETS ${LOCAL_TARGET_NAME_SHARED}
				RUNTIME DESTINATION ${JVX_SHARED_LIB_TARGET_FOLDER}
				LIBRARY DESTINATION ${INSTALL_PATH_LIB_SHARED} COMPONENT ${INSTALL_COMPONENT})
			
			# Install PCB file if it exists
			install(FILES $<TARGET_PDB_FILE:${LOCAL_TARGET_NAME_SHARED}> DESTINATION ${JVX_SHARED_LIB_TARGET_FOLDER} OPTIONAL)
			
		else()
			install(TARGETS ${LOCAL_TARGET_NAME_SHARED}
				RUNTIME DESTINATION ${INSTALL_PATH_LIB_SHARED}
				LIBRARY DESTINATION ${INSTALL_PATH_LIB_SHARED} COMPONENT ${INSTALL_COMPONENT})
				
			message("11111 -> ${INSTALL_PATH_LIB_SHARED} -- ${INSTALL_PATH_LIB_SHARED}")
		endif()
		
		if(JVX_ALSO_INSTALL_IMPORT_LIB)
			install(TARGETS ${LOCAL_TARGET_NAME_SHARED}
				ARCHIVE DESTINATION ${INSTALL_PATH_LIB_IMPORT} COMPONENT ${INSTALL_COMPONENT})
		endif()		
    endif()

    if(JVX_USES_SDK)

      # In case we want to install an import lib (what happens in Linux?)
		if(INSTALL_IMPORT_LIB)
			install(TARGETS ${LOCAL_TARGET_NAME_SHARED}
				ARCHIVE DESTINATION ${INSTALL_PATH_LIB_STATIC}
				COMPONENT ${INSTALL_COMPONENT})
		endif()
    endif()
    
    # The corresponding start script
    if(LOCAL_START_SCRIPT)

      # This install is only allowed if we are not within SDK build
		if(JVX_RELEASE_SDK)
			message(FATAL_ERROR "Project ${JVX_TARGET_NAME} run to produce application while compiling for SDK release")
		endif()
		message("##### Installing local start script ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/${LOCAL_START_SCRIPT}${JVX_SCRIPT_EXTENSION}")
		install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${JVX_OS}/${LOCAL_START_SCRIPT}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
    endif(LOCAL_START_SCRIPT)

    if(CONFIGURE_LOCAL_START_SCRIPT)
		# This install is only allowed if we are not within SDK build
		if(JVX_RELEASE_SDK)
			message(FATAL_ERROR "Project ${JVX_TARGET_NAME} run to produce application while compiling for SDK release")
		endif()
		configure_file(${JVX_BASE_ROOT}/software/templates/scripts/start${JVX_SCRIPT_EXTENSION}.${JVX_OS}.in ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
    endif(CONFIGURE_LOCAL_START_SCRIPT)

  endif()
endif()

if(NOT JVX_RELEASE_SDK)
	if(LOCAL_START_SCRIPT_MATLAB)
		message("Configuring file ${LOCAL_START_SCRIPT_OCTAVE}${JVX_SCRIPT_EXTENSION}.in for Matlab start script")
		if(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME)
			configure_file(${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in 
				${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION})
			install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME}${JVX_SCRIPT_EXTENSION}  DESTINATION ${INSTALL_PATH_SCRIPTS})			
		else(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME)
			message(" ++  > ${LOCAL_START_SCRIPT_MATLAB}")
			configure_file(${LOCAL_START_SCRIPT_MATLAB}${JVX_SCRIPT_EXTENSION}.in 
				${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}-mat${JVX_SCRIPT_EXTENSION})
			install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}-mat${JVX_SCRIPT_EXTENSION}  DESTINATION ${INSTALL_PATH_SCRIPTS})
		endif(LOCAL_START_SCRIPT_MATLAB_SCRIPT_NAME)
		
  else(LOCAL_START_SCRIPT_MATLAB)
	if(CONFIGURE_LOCAL_START_SCRIPT)
		set(JVX_MATLAB_EXECUTABLE_REPLACE ${Matlab_MAIN_PROGRAM})
		message("Producing file ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION}")
		configure_file(${JVX_BASE_ROOT}/software/templates/scripts/startMatlab${JVX_SCRIPT_EXTENSION}.${JVX_OS}.in ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION})
		install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/start-${JVX_TARGET_NAME}${JVX_SCRIPT_EXTENSION} DESTINATION ${INSTALL_PATH_SCRIPTS} COMPONENT ${INSTALL_COMPONENT})
	endif(CONFIGURE_LOCAL_START_SCRIPT)
  endif(LOCAL_START_SCRIPT_MATLAB)
endif(JVX_RELEASE_SDK)
  
###
# error
###
if(NOT BUILD_SHARED AND NOT BUILD_STATIC)
  message(FATAL_ERROR "Library module must define BUILD_STATIC or BUILD_SHARED")
endif()

if(DEFINED JVX_COLLECT_INSTALL_DIRECTORIES)
	message("JVX_COLLECT_INSTALL_DIRECTORIES = ${JVX_COLLECT_INSTALL_DIRECTORIES}")
else()
	message("JVX_COLLECT_INSTALL_DIRECTORIES does not exist, setting value to false.")
	set(JVX_COLLECT_INSTALL_DIRECTORIES FALSE)
endif()

jvx_install_project_folders(${JVX_COLLECT_INSTALL_DIRECTORIES})

if(DEFINED JVX_SUBPRODUCT_ROOT)
  include(${JVX_SUBPRODUCT_ROOT}/cmake/lib.${JVX_PRODUCT}.cmake)
endif()

if(DEFINED JVX_SDK_PATH)
	include(${JVX_SDK_PATH}/cmake/standalone.cmake)
endif(DEFINED JVX_SDK_PATH)

set(PROJECT_NAME jvx-ntask)
include(${JVX_CMAKE_DIR}/common_local_lib_head.cmake)

# configure_file(include/jvx_dsp.h.in ${CMAKE_CURRENT_BINARY_DIR}/jvx_dsp.h)

# add a target to generate API documentation with Doxygen
# FIXME: support in common foot for this?
if(DOXYGEN_FOUND AND LATEX_FOUND AND LATEX_DVIPS_FOUND)
  message("--> Running documentation generation in project <${PROJECT_NAME}>.")

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(jvx-ntask-doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
    )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/os-${JVX_OS}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(LOCAL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/jvx_ntask.c
	${CMAKE_CURRENT_SOURCE_DIR}/include/jvx_ntask.h
	)

if( JVX_USE_PART_MATLAB AND JVX_GENERATE_HEADER_ON_BUILD)
	#set(LOCAL_MATCONF_SCRIPTS 
	#	${CMAKE_CURRENT_SOURCE_DIR}/src/hilbert_filter/generate_config_header.m
	#	)
	#set(LOCAL_MATCONF_ADDITIONAL_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../jvx-nio-matlab)
else()
	#message("--> Copying files from pre-generated folder")
	#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_nio_hilbert_1can.h DESTINATION 
	#	${CMAKE_CURRENT_BINARY_DIR}/generated)
	#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pre-generated/jvx_nio_hilbert_2can.h DESTINATION 
	#	${CMAKE_CURRENT_BINARY_DIR}/generated)	
endif()

  
set(LOCAL_LIBS jvx-dsp-base_static)

#set(LOCAL_COMPILE_DEFINITIONS "${FFT_COMPILE_DEFINITIONS}")

set(BUILD_STATIC TRUE)
set(IS_C_LIB TRUE)
set(INSTALL_DEDICATED_INCLUDE_TREE TRUE)
set(INSTALL_DEDICATED_GENERATED_TREE TRUE)

include(${JVX_CMAKE_DIR}/common_local_lib_foot.cmake)

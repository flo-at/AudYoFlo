## ======================================================================
## The audio libs
if(JVX_USE_MFVIDEO)
	include_directories(${JVX_SUBPRODUCT_BINARY_DIR}/sources/jvxComponents/jvxVideoTechnologies/jvxViTMfOpenGL)
	set(LOCAL_COMPILE_DEFINITIONS "${LOCAL_COMPILE_DEFINITIONS};JVX_USE_MFVIDEO")
	set(LOCAL_LIBS ${LOCAL_LIBS} jvxViTMfOpenGL_static)
endif()
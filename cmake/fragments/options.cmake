# build options
option(JVX_COMPILE_BUILDTOOLS "Create project to build jvx build tools (pcg & mcg)" OFF)
option(JVX_USE_PART_CMINPACK "Compile with cminpack lib support" OFF)
option(JVX_USE_PART_MATLAB "Compile with Matlab support" OFF)
option(JVX_USE_PART_OCTAVE "Compile with Octave support" OFF)
option(JVX_USE_PART_ASIO  "Compile with ASIO support (MacOS(?) and Windows only)" OFF)
option(JVX_USE_PART_RS232 "Compile with RS232 support" OFF)
option(JVX_USE_PART_CORECONTROL "Compile with CoreControl support" OFF)
option(JVX_USE_QT "Compile with QT support" OFF)
option(JVX_USE_QWT "Compile with QWT support" OFF)
option(JVX_USE_QWT_POLAR "Compile with QWT-Polar support" OFF)
option(JVX_DEPLOY_LIBRARY_DSP_MATLAB "Install Matlab/Octave DSP-lib files" OFF)
option(JVX_DEPLOY_UNSTABLE "Install unstable components" OFF)
option(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT "Use single precision floating point as standard datatype (instead of double)" OFF)
option(JVX_MATLAB_PROPERTY_GENERATOR "Generate Matlab files to access component properties" OFF)
option(JVX_USE_PCAP "Allow pcap socket in clients and servers" OFF)
option(JVX_CRYPT_MATLAB_CODE "Convert all *.m files into *.p files in case code is released to customers" OFF)
option(JVX_USE_OLD_CONFIG_FILE_TYPE "Use old config file style" OFF)
## option(JVX_COMPILE_BUILDTOOLS "Compile required buildtools only (prerequisite for regular build)" OFF)
option(JVX_RELEASE_OMAPL137 "Compile omap137 DSP applications (requires CCS and DSP/BIOS)" OFF)
option(JVX_RELEASE_C674X "Compile c674x DSP applications (requires CCS and DSP/BIOS)" OFF)
option(JVX_RELEASE_ARMCORTEXA8 "Compile ARM Cortex A8 applications (requires CCS and ARM-GCC)" OFF)
option(JVX_RELEASE_ARMCORTEXM4 "Compile ARM Cortex M4 applications (requires NXP MCUXpresso)" OFF)
option(JVX_USE_OPENGL "Compile applications and components using opengl in combination with x11 (freeglut and glew)" OFF)
option(JVX_USE_OPENGL_X11 "Compile applications and components using opengl in combination with x11 (freeglut and glew)" OFF)
option(JVX_USE_OPENCV "Compile applications and components using opencv" OFF)
option(JVX_USE_BOOST "Find boost - compilation is not required since it is a header lib" OFF)
option(JVX_USE_EIGEN "Find Eigen - compilation is not required since it is a header lib" OFF)
option(JVX_DOWNLOAD_BOOST "Find boost - compilation is not required since it is a header lib" OFF)
option(JVX_DOWNLOAD_EIGEN "Find Eigen - compilation is not required since it is a header lib" OFF)
option(JVX_DEPLOY_EXAMPLES "Deploy the example projects for platform." OFF)
option(JVX_GENERATE_HEADER_ON_BUILD "Generate headers while building overall project." ON)
option(JVX_OBJECTS_WITH_TEXTLOG "All objects have automatic access to the host text log" ON)
option(JVX_USE_PART_WEBSERVER "Add civet web webserver" OFF)
option(JVX_USE_CONSOLE_APPS "Activate to use the console attached to each process (win32)" OFF)
option(JVX_COMPILE_GOD_HOST "Compile the host version to load all dlls (Gods host)" OFF)
option(JVX_TARGET_GCC_ARM_YOCTO "Show that this build runs on an ARM core to activate ARM optimization." OFF)
option(JVX_SPLASH_SUPPRESSED "Option to suppress all splash popups in QT" OFF)
option(JVX_OPENGL_MAP_GLES2 "Option to replace all GL functions to operate in GLES2" OFF)
option(JVX_FORCE_GENERATE_NEW_VERSION_TAG "Force to generate version tag header at every run of cmake" OFF)
option(JVX_GENERATE_DEPEND_GRAPH "Create dependency graph while building" OFF)
option(JVX_COMPILE_QT_EGLFS_TAG "Special tag to compile qt applications to run on eglfs framebuffer (QT). This may be used to generate specific start scripts" OFF)
option(JVX_COMPILE_FOR_PI_TAG "Tag to compile for PI to activate specific aplications only available on PI" OFF)
option(JVX_WIDGET_WRAPPER_INFO_UPDATE "Activate additional info in widget wrapper elements to show update state" OFF)
option(JVX_WIDGET_WRAPPER_PENDING_UPDATE "Activate verbose output in widget wrapper elements to show if pending update causes skip of hhtp request" OFF)
option(JVX_GLOBAL_BUFFERING_VERBOSE "Activate printout on buffering issues" OFF)
option(JVX_USE_PART_CUDA "Compile CUDA modules" OFF)
option(JVX_PERMIT_QCP_OPENGL "Compile CUDA modules" ON)
option(JVX_FORCE_CPP_14 "Force to use only C++14 standard" OFF)
option(JVX_FORCE_C_11  "Force to use C11 standard" OFF)
option(JVX_USE_AVX "Allow the compiler to use the AVX instruction sets" OFF)
option(JVX_RELEASE_SDK_SUPPRESS_HEADER_INSERTION "Disengage header insertion SDK build" ON)
option(JVX_SDK_FORCE_SHARED_LINKAGE "Force deployment of dll components with import lib" OFF)
option(JVX_QT_VERSION_6 "Modify cmake run to support QT6" OFF)
option(JVX_USE_PYTHON "Build all python modules" OFF)
option(JVX_USE_FFTW "Build with fftw" ON)
option(JVX_CIVETWEB_INCLUDED_AS_SOURCE "Integrate civetweb source code in web server." ON)
option(JVX_USE_FFMPEG "Use ffmpeg library" OFF)
option(JVX_USE_CCACHE "Build with ccache support" ON)
option(JVX_INSTALL_QT_ON_BUILD "Download and install QT on first build" ON)
option(JVX_INSTALL_FLUTTER_ON_BUILD "Download and install Flutter on first build" ON)
option(JVX_ENGAGE_TEST_EXAMPLES "Engage test example applications" OFF)
option(JVX_TRACK_MEMORY_LEAKS "Better track memory leaks - mainly in windows." OFF)
option(JVX_GCC_LINKER_SYMBOLIC "Activate symbolic link option for gcc - required by ffmpeg in linux." OFF)
option(JVX_USE_SIMULINK "Activate Simulink (Matlab) projects." OFF)
option(JVX_BUILD_FOR_WINDOWS_XP "Activate defines for Windows XP." OFF)


set(JVXRT_SUBMODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sources/sub-projects" CACHE STRING "Path to submodule folder")
set(JVXRT_SUBMODULE_FLUTTER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/flutter" CACHE STRING "Path to flutter submodules")

set(JVX_DEFINE_NUMBER_32BIT_TOKENS_BITFIELD 16 CACHE STRING "Number of 32 bit tokens in bitfields")
set(JVX_MAX_PATH_PROP_MAT 63 CACHE STRING "Number of characters when generating Matlab property access functions")
set(JVX_DEFAULT_NUMBER_OF_POSTPONE_ATTEMPTS_TEST 10 CACHE STRING "Number of reattempts if chain element returns a POSTPONE")
set(JVX_COMMAND_LINE_TOKENS_BUILDIN "--textlog --verbose_dll --textloglev 10" CACHE STRING "Pre-compiled builtin command args")
set(JVX_QT_SPLASH_TEXT_COLOR "Qt::gray" CACHE STRING "Default color for QT apps splash screen text")
set(JVX_SUBMODULE_PASS_EXPRS "default;pass0;pass1;pass2" CACHE STRING "Submodule pass expression list.")

# set(AYF_WASM_INSTALL_FOLDER "" CACHE STRING "Wasm install folder")

# Options for the pure audio part
option(JVX_INCLUDE_EBUR128 "Include ebur128 lib to build" OFF)
option(JVX_USE_PART_CMINPACK "Compile with cminpack lib support" OFF)
option(JVX_USE_PART_PAUDIO "Compile with PortAudio support" OFF)
option(JVX_USE_PART_ALSA  "Compile with ALSA support (Linux only)" OFF)
option(JVX_USE_PART_COREAUDIO  "Compile with Coreaudio support (MacOS only)" OFF)
option(JVX_USE_PART_WASAPI  "Compile with WASAPI support (Windows only)" OFF)
option(JVX_USE_BOOST "Find boost - compilation is not required since it is a header lib" OFF)
option(JVX_USE_EIGEN "Find Eigen - compilation is not required since it is a header lib" OFF)
option(JVX_USE_HDF5 "Find HDF5 precompiled binary" OFF)
option(JVX_USE_VST "Find VST sdk examples" OFF)
option(JVX_USE_PART_MPG123 "Find mpg123 mp3 decoder library" OFF)
option(JVX_FLUTTER_UI "Compile flutter ffi lib" OFF)
option(JVX_LIBHOST_WITH_JSON_SHOW "Enable json show extensions in lib host" OFF)
option(JVX_USE_LIBMYSOFA "Compile with libmysofa support" OFF)
option(JVX_USE_ORC "Find ORC" OFF)
option(JVX_USE_KISSFFT "Involve kissfft" OFF)

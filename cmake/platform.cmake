# set platform variables
message("--> OS is ${CMAKE_SYSTEM_NAME}")
if(NOT DEFINED CMAKE_SIZEOF_VOID_P)
  set(CMAKE_SIZEOF_VOID_P  4)
  message(WARNING "XX> Platform type not set, defaulting to 32 Bit")
endif()
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  message("--> 32 Bit platform")
  set(JVX_PLATFORM "32bit")
else()
  message("--> 64 Bit platform")
  set(JVX_PLATFORM "64bit")
endif()

message("CMake System detection: ${CMAKE_SYSTEM_NAME}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(IOS)
    set(JVX_OS "ios")
  else()
    set(JVX_OS "macosx")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(${MINGW})
		set(JVX_OS "mingw")
	else()
		set(JVX_OS "windows")
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(JVX_OS "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
     set(JVX_OS "ios")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
     set(JVX_OS "Emscripten")
else()
  message(FATAL_ERROR "${CMAKE_SYSTEM_NAME}: this os is not supported")
endif()

set(JVX_SYSTEM_TOKEN "JVX_SYS")

#if(${UNIX})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_UNIX")
#elseif(${WIN32})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_WIN32")
#elseif(${APPLE})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_APPLE")
#else()
#	message(FATAL_ERROR "Unexpected OS")
#endif()

string(TOUPPER ${JVX_OS} JVX_OS_CAPITAL)
string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_" ${JVX_OS_CAPITAL})

#if(${MINGW})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_MINGW")
#elseif(${MSYS})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_MSYS")
#elseif(${CYGWIN})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_CYGWIN")
#elseif(${BORLAND})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_BORLAND")
#elseif(${WATCOM})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_WATCOM")
#elseif(${MSVC})
#	string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_MSVC")
#else()
#	message(FATAL_ERROR "Unexpected Build Environment")
#endif()

string(TOUPPER ${CMAKE_C_COMPILER_ID} CMAKE_C_COMPILER_ID_CAPITAL)
string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_" ${CMAKE_C_COMPILER_ID_CAPITAL})

string(TOUPPER ${JVX_PLATFORM} JVX_PLATFORM_CAPITAL)
string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_" ${JVX_PLATFORM_CAPITAL})

# -> We may need to change this variable for cross compilation!! Is this reall needed or does the compiler handle all of this?
# string(CONCAT JVX_SYSTEM_TOKEN ${JVX_SYSTEM_TOKEN} "_" ${CMAKE_SYSTEM_PROCESSOR})

message("--> JVX_SYSTEM_TOKEN: ${JVX_SYSTEM_TOKEN}")
set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};${JVX_SYSTEM_TOKEN}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};_GNU_SOURCE")
endif()

if(JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
	message("--> Compiling for SINGLE precision float")
else()
	message("--> Compiling for DOUBLE precision float")
endif()

# We can run the crosscompile build either on Linux or on Windows (Emspripten)
# The executable name must be adapted accordingly
set(JVX_EXECUTABLE_EXTENSION_OPERATE "")
cmake_host_system_information(RESULT JVX_OS_NAME QUERY OS_NAME)
if(${JVX_OS_NAME} MATCHES "Windows")
	set(JVX_EXECUTABLE_EXTENSION_OPERATE ".exe")
endif()
message("Running on system ${JVX_OS_NAME}, the tool executable extension is <${JVX_EXECUTABLE_EXTENSION_OPERATE}>")

if(JVX_TRACK_MEMORY_LEAKS)
	set(GLOBAL_COMPILE_DEFINITIONS "${GLOBAL_COMPILE_DEFINITIONS};JVX_TRACK_MEMORY_LEAKS")
endif()

# include platform specific stuff
include(${JVX_CMAKE_DIR}/${JVX_OS}.cmake)

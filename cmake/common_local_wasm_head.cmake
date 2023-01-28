# Buiding web assembly in cmake:
# https://gist.github.com/WesThorburn/00c47b267a0e8c8431e06b14997778e4
cmake_minimum_required(VERSION 3.5.1)

message("--> Configuring ${PROJECT_NAME}")
set(JVX_TARGET_NAME ${PROJECT_NAME})
set(JVX_MODULE_NAME ${PROJECT_NAME})
project (${JVX_TARGET_NAME})

if(NOT JVX_SYSTEM_USE_DATA_FORMAT_FLOAT)
	message(FATAL_ERROR "Wasm modules only support 32 bit float arithmentic. You need to set the option <JVX_SYSTEM_USE_DATA_FORMAT_FLOAT> to TRUE!")
endif()

set(JVX_EMSCRIPTEN_OUTPUT_MODE_DEBUG FALSE)
set(JVX_EMSCRIPTEN_WASM_ASSEMBLY TRUE)

# In audio worklets, always use 
set(JVX_EMSCRIPTEN_SINGLEFILE TRUE)
set(JVX_EMSCRIPTEN_SYNC_LOAD TRUE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(JVX_EMSCRIPTEN_OUTPUT_MODE_DEBUG TRUE)
	set(CMAKE_CXX_FLAGS_LOCAL "${CMAKE_CXX_FLAGS_RELEASE}")
else()
	set(JVX_EMSCRIPTEN_OUTPUT_MODE_DEBUG FALSE)
	set(CMAKE_CXX_FLAGS_LOCAL "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

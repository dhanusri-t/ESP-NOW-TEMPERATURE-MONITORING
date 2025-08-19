# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Ensure IDF_PATH is set correctly before including project.cmake
if(NOT DEFINED ENV{IDF_PATH})
	message(FATAL_ERROR "IDF_PATH environment variable is not set. Please set it to your ESP-IDF directory.")
endif()
include("$ENV{IDF_PATH}/tools/cmake/project.cmake")
project(espnow_central_node)

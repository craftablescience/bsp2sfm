cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# set up project
project(bsp2sfm
        DESCRIPTION "Convert Team Fortress 2 maps to the format Source Filmmaker expects."
        VERSION 1.0.0
        HOMEPAGE_URL "https://github.com/craftablescience/bsp2sfm")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add helpers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers")
include(CS_All)

# extra metadata variables
set(PROJECT_NAME_PRETTY "${PROJECT_NAME}" CACHE STRING "" FORCE)
cs_version_pretty() # PROJECT_VERSION_PRETTY
set(PROJECT_ORGANIZATION_NAME "craftablescience" CACHE STRING "" FORCE)
set(PROJECT_GITHUB_URL "${PROJECT_HOMEPAGE_URL}" CACHE STRING "" FORCE)

# initial VS integration
cs_setup_vs_defaults(STARTUP_PROJECT "${PROJECT_NAME}")

# set common compile options
option(BSP2SFM_USE_LTO "Compile ${PROJECT_NAME_PRETTY} with LTO/IPO" OFF)
cs_configure_defaults(BSP2SFM_USE_LTO)

# external libraries
cs_include_directory(ext)

# common documents and config
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/LICENSE" COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/common/Config.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Config.hpp")

# bsp2sfm
cs_include_directory(src/cli)

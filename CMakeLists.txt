cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(ProgParam VERSION      0.01.00
        DESCRIPTION "to managed the parameters and programs that form a software and transparently join the core functionality with the user interface."
        HOMEPAGE_URL https://github.com/qPCR4vir/ProgParam
        LANGUAGES CXX         )

#######################     Main setting of targets, sources and installs

add_library(progparam)
target_compile_features   (progparam PUBLIC cxx_std_17)
target_include_directories(progparam PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_sources            (progparam PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/init_prog_param.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ParamGUIBind.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/Units.cpp)


if (NANA_CMAKE_VERBOSE_PREPROCESSOR)

    include(CMakePrintHelpers)
    cmake_print_properties(TARGETS progparam  PROPERTIES
            COMPILE_DEFINITIONS   COMPILE_OPTIONS    COMPILE_FLAGS  LINK_LIBRARIES
            INCLUDE_DIRECTORIES   INSTALL_NAME_DIR   LINK_FLAGS     VERSION)

endif()
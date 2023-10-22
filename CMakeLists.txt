# Specify the minimum required version of CMake and project details
cmake_minimum_required(VERSION 3.10)
project(mysql_vss VERSION 0.0.1)

set(MYSQL_VSS_VERSION ${PROJECT_VERSION})

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/build/config.h)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for the plugin
set(SOURCE_FILES 
    src/mysql_vss.cc 
    src/vss_search.cc
    src/vss_version.cc
)

# Include directories
include_directories(
    ${CMAKE_BINARY_DIR}/build
    /usr/local/opt/openssl/include
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src
    ${CMAKE_SOURCE_DIR}/src/vendor/annoy-1.17.3/src
    /Library/Frameworks/Python.framework/Headers/
)

# Link directories
link_directories(/usr/local/opt/openssl/lib)

# Add the plugin as a shared library
add_library(mysql_vss MODULE ${SOURCE_FILES})
target_compile_definitions(mysql_vss PRIVATE MYSQL_DYNAMIC_PLUGIN)

# Specify the include directories for mysql_vss
target_include_directories(mysql_vss PRIVATE  
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/include
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/include
)

find_library(MYSQLCLIENT_LIBRARY 
             NAMES mysqlclient 
             PATHS ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/lib
             NO_DEFAULT_PATH)

if(NOT MYSQLCLIENT_LIBRARY)
    message(FATAL_ERROR "MySQL client library not found!")
endif()

# Annoy library files
set(ANNOY_FILES
    ${CMAKE_SOURCE_DIR}/src/vendor/annoy-1.17.3/src/annoymodule.cc
)
add_library(annoy ${ANNOY_FILES})

# Jansson library files
set(JANSSON_FILES
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src/dump.c
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src/hashtable.c
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src/load.c
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src/strbuffer.c
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src/utf.c
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src/value.c
)
add_library(jansson ${JANSSON_FILES})

target_link_libraries(mysql_vss PRIVATE 
    annoy
    jansson
    ${MYSQLCLIENT_LIBRARY}
)
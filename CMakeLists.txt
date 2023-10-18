# Specify the minimum required version of CMake and project details
cmake_minimum_required(VERSION 3.10)
project(mysql_vss VERSION 0.0.1)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for the plugin
set(SOURCE_FILES 
    src/mysql_vss.cc 
    src/vss_search.cc
    src/vss_version.cc
)

# Include directories
include_directories(
    /usr/local/opt/openssl/include
    ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src
)

# Link directories
link_directories(/usr/local/opt/openssl/lib)

# Add the plugin as a shared library
add_library(mysql_vss MODULE ${SOURCE_FILES})

# Specify the include directories for mysql_vss
target_include_directories(mysql_vss PRIVATE  
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/include
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/include
)

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

# Link mysql_vss with jansson
target_link_libraries(mysql_vss jansson)

# If you decide to install the plugin to the MySQL plugin directory later, you can uncomment and use the following:
# install(TARGETS mysql_vss DESTINATION path_to_mysql_plugin_directory)

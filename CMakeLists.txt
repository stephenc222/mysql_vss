# Specify the minimum required version of CMake and project details
cmake_minimum_required(VERSION 3.10)
project(mysql_vss VERSION 0.0.1)

set(MYSQL_VSS_VERSION ${PROJECT_VERSION})

if(APPLE)
    set(OS_NAME "macOS")
elseif(UNIX AND EXISTS "/etc/system-release")
    file(READ "/etc/system-release" OS_RELEASE)
    if(OS_RELEASE MATCHES "Amazon Linux release 2023")
        set(OS_NAME "AmazonLinux2023")
    else()
        set(OS_NAME "Linux")
    endif()
else()
    set(OS_NAME "UnknownOS")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64|x86_64")
    set(CPU_ARCH "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(CPU_ARCH "arm_64")
else()
    set(CPU_ARCH "UnknownArch")
endif()


configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/build/config.h)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Determine library extension based on platform
if(APPLE)
    set(CMAKE_SHARED_MODULE_SUFFIX ".dylib")
endif()


# Source files for the plugin
set(SOURCE_FILES 
    src/annoy_service.cc
    src/mysql_vss.cc 
    src/vss_search.cc
    src/vss_version.cc
)

# Include directories
if(APPLE)
    set(CMAKE_SHARED_MODULE_SUFFIX ".dylib")
    include_directories(
        ${CMAKE_BINARY_DIR}/build
        ${CMAKE_SOURCE_DIR}/src/include
        /usr/local/opt/openssl/include
        ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src
        ${CMAKE_SOURCE_DIR}/src/vendor/annoy-1.17.3/src
        /Library/Frameworks/Python.framework/Headers/
    )
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    include_directories(
        ${CMAKE_BINARY_DIR}/build
        ${CMAKE_SOURCE_DIR}/src/include
        /usr/include/openssl
        ${CMAKE_SOURCE_DIR}/src/vendor/jansson-1.0.1/src
        ${CMAKE_SOURCE_DIR}/src/vendor/annoy-1.17.3/src
        /usr/include/python3.9
    )
endif()


find_package(OpenSSL REQUIRED)


# Add the plugin as a shared library
add_library(mysql_vss MODULE ${SOURCE_FILES})
target_compile_definitions(mysql_vss PRIVATE MYSQL_DYNAMIC_PLUGIN)

# Check the compiler and set the UTF-8 flags accordingly
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(mysql_vss PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mysql_vss PRIVATE /utf-8)
endif()

# Set the output name to include version and operating system
set_target_properties(mysql_vss PROPERTIES
    OUTPUT_NAME "mysql_vss_v${MYSQL_VSS_VERSION}_${OS_NAME}_${CPU_ARCH}"
)

# Check if the compiler is GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_target_properties(mysql_vss PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()


# Specify the include directories for mysql_vss
target_include_directories(mysql_vss PRIVATE  
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/include
    ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/include
)

if(APPLE)
    find_library(MYSQLCLIENT_LIBRARY 
                NAMES mysqlclient 
                PATHS ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/lib
                NO_DEFAULT_PATH)
else()
    # set(MYSQLCLIENT_LIBRARY ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/lib/libmysqlclient.so.21)
    set(MYSQLCLIENT_LIBRARY ${CMAKE_SOURCE_DIR}/src/vendor/mysql-server/build/archive_output_directory/libmysqlclient.a)
endif()

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
add_library(jansson STATIC ${JANSSON_FILES})

target_link_libraries(mysql_vss PRIVATE 
    ${OPENSSL_LIBRARIES}
    annoy
    jansson
    ${MYSQLCLIENT_LIBRARY}
)

# If compiler is GCC, add static link flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(mysql_vss PRIVATE 
        -static-libgcc
        -static-libstdc++
    )
endif()

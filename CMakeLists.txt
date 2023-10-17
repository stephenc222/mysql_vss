
ADD_LIBRARY(jansson
${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src/dump.c
${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src/hashtable.c
${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src/load.c
${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src/strbuffer.c
${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src/utf.c
${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src/value.c
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/plugin/mysql_vss/src/vendor/jansson-1.0.1/src)



MYSQL_ADD_PLUGIN(mysql_vss src/mysql_vss.cc 
    src/vss_search.cc
    src/vss_version.cc
    MODULE_ONLY)


TARGET_LINK_LIBRARIES(mysql_vss jansson)
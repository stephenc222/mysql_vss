/*
MySQL VSS Plugin

A MySQL plugin to store and query vector embeddings
*/

#include "include/mysql_vss.h"
#include "mysql/plugin.h"

// Plugin initialization function
static int mysql_vss_plugin_init(MYSQL_PLUGIN plugin_info)
{
  return 0; // Indicate success
}

// Plugin deinitialization function
static int mysql_vss_plugin_deinit(MYSQL_PLUGIN plugin_info) { return 0; }

// Plugin type-specific descriptor
struct st_mysql_daemon mysql_vss_descriptor = {MYSQL_DAEMON_INTERFACE_VERSION};

// MySQL plugin descriptor
mysql_declare_plugin(mysql_vss){
    MYSQL_DAEMON_PLUGIN,
    &mysql_vss_descriptor,
    "mysql_vss",
    "Stephen Collins",
    "MySQL VSS plugin",
    PLUGIN_LICENSE_GPL,
    mysql_vss_plugin_init, // Plugin Init Function
    nullptr,
    mysql_vss_plugin_deinit, // Plugin Deinit Function
    0x0001,                  // Plugin version
    nullptr,                 // Status Vars
    nullptr,                 // System Vars
    nullptr,                 // Reserved
    0,                       // Flags
} mysql_declare_plugin_end;

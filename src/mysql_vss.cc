#include "include/mysql_vss.h"

// TODO: Consider how / if to combine a MySQL plugin to support the vss_search loadable function

static int mysql_vss_plugin_init(MYSQL_PLUGIN plugin_info)
{
  return 0;
}

static int mysql_vss_plugin_deinit(MYSQL_PLUGIN plugin_info) { return 0; }

struct st_mysql_daemon mysql_vss_descriptor = {MYSQL_DAEMON_INTERFACE_VERSION};

// MySQL plugin descriptor
mysql_declare_plugin(mysql_vss){
    MYSQL_DAEMON_PLUGIN,
    &mysql_vss_descriptor,
    "mysql_vss",
    "Stephen Collins",
    "MySQL VSS plugin",
    PLUGIN_LICENSE_GPL,
    mysql_vss_plugin_init,
    nullptr,
    mysql_vss_plugin_deinit,
    0x0001,
    nullptr,
    nullptr,
    nullptr,
    0,
} mysql_declare_plugin_end;

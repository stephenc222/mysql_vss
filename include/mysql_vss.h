#ifndef VSS_PLUGIN_H
#define VSS_PLUGIN_H

#include <fstream>
#include <math.h>
#include "mysql.h"
#include <mysql/plugin.h>
#include "annoy-1.17.3/src/annoylib.h"
#include "annoy-1.17.3/src/kissrandom.h"
#include "include/annoy_service.h"

extern "C"
{

  char *vss_search(UDF_INIT *initid, UDF_ARGS *args,
                   char *result, unsigned long *length,
                   char *is_null, char *error);
  bool vss_search_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
  void vss_search_deinit(UDF_INIT *initid);

  char *vss_version(UDF_INIT *initid, UDF_ARGS *args, char *result,
                    unsigned long *length, char *is_null, char *error);
  bool vss_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
  void vss_version_deinit(UDF_INIT *initid);
}

#endif // VSS_PLUGIN_H

#ifndef VSS_PLUGIN_H
#define VSS_PLUGIN_H

#include <mysql/plugin.h>
#include "annoylib.h"
#include "kissrandom.h"
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

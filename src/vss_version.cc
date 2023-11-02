#include <mysql/plugin.h>
#include "config.h"

extern "C"
{
  char *vss_version(UDF_INIT *initid, UDF_ARGS *args, char *result,
                    unsigned long *length, char *is_null, char *error)
  {
    const char *version = MYSQL_VSS_VERSION;
    *length = strlen(version);
    strcpy(result, version);
    result[*length] = '\0';
    return result;
  }

  bool vss_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
  {
    return 0;
  }

  void vss_version_deinit(UDF_INIT *initid) {}
}

#include <mysql/plugin.h>

extern "C" {
char *vss_version(UDF_INIT *initid, UDF_ARGS *args, char *result,
                  unsigned long *length, char *is_null, char *error) {
  // TODO: proper version handling
  const char *version = "0.0.1";
  *length = strlen(version);
  strcpy(result, version);
  result[*length] = '\0';
  return result;
}

bool vss_version_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  return 0;
}

void vss_version_deinit(UDF_INIT *initid) {}
}

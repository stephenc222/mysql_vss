#include <mysql/plugin.h>

extern "C" {
long long vss_search(UDF_INIT *initid, UDF_ARGS *args, char *result,
                     unsigned long *length, char *is_null, char *error) {
  return 42;
}

bool vss_search_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  return 0;
}

void vss_search_deinit(UDF_INIT *initid) {}
}

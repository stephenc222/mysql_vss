#include <mysql/plugin.h>

// TODO: to expose to the user a way to "update/add" embeddings on demand.

extern "C"
{
  long long *vss_update(UDF_INIT *initid, UDF_ARGS *args,
                        char *is_null, char *error)
  {
    return 0;
  }

  bool vss_update_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
  {
    return 0;
  }

  void vss_update_deinit(UDF_INIT *initid) {}
}

#include <fstream>
#include <math.h>
#include "mysql/plugin.h"
#include "mysql.h"
#include "jansson.h"
#include "include/mysql_vss.h"

extern "C"
{

  // TODO: consider allocate memory for this index structure in the plugin not this UDF?
  // To be able to directly deallocate on uninstall plugin?
  // Global Annoy index variable

  char *vss_search(UDF_INIT *initid, UDF_ARGS *args,
                   char *result, unsigned long *length,
                   char *is_null, char *error)
  {

    if (args->arg_count != 1)
    {
      strcpy(error, "One string argument (vector) is required.");
      *is_null = 1;
      return NULL;
    }

    AnnoyService &populator = AnnoyService::getInstance();

    return populator.get_closest(error, args->args[0], result, length, is_null);
  }

  bool vss_search_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
  {
    if (args->arg_count != 1)
    {
      strcpy(message, "One string argument (vector) is required.");
      return 1; // Indicate error
    }

    AnnoyService &populator = AnnoyService::getInstance();
    return populator.load_index(message);
  }

  void vss_search_deinit(UDF_INIT *initid)
  {
    return; // Indicate success
  }
}

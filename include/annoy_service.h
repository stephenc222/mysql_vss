#ifndef ANNOY_SERVICE_H
#define ANNOY_SERVICE_H

#include <fstream>
#include <map>
#include <sstream>
#include "mysql/plugin.h"
#include "mysql.h"
#include "annoy-1.17.3/src/kissrandom.h"
#include "annoy-1.17.3/src/annoylib.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

// FIXME: either dynamically pull this while loading the index or make it configurable
// during UDF function creation. Currently will only work for an embedding model that
// outputs vectors with 768 dimensions.
#ifndef EMBEDDING_DIM
#define EMBEDDING_DIM 768
#endif

#ifndef ANNOY_INDEX_PATH
#define ANNOY_INDEX_PATH "mysql_vss_annoy.index"
#endif

using namespace Annoy;

class AnnoyService
{
private:
  // NOTE: quick-fix solution for database configuration for SQL queries. Alternative
  // could be using the internal MySQL C API, leveraging the execution context to create statements
  std::map<std::string, std::string> dbConfig;
  Annoy::AnnoyIndex<int, double, Annoy::Angular, Annoy::Kiss32Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy> *annoy_index;
  AnnoyService();
  AnnoyService(const AnnoyService &) = delete;
  AnnoyService &operator=(const AnnoyService &) = delete;

public:
  static AnnoyService &getInstance();
  ~AnnoyService();
  char *get_closest(char *error, char *vector_arg, char *result, unsigned long *length, char *is_null);
  bool load_index(char *message);
  void populate_annoy_from_db();
};

#endif // ANNOY_SERVICE_H

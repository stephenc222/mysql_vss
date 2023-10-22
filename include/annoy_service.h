#ifndef ANNOY_SERVICE_H
#define ANNOY_SERVICE_H

#include "mysql/plugin.h"
#include "mysql.h"
#include "jansson.h"
#include "kissrandom.h"
#include "annoylib.h"
#include <fstream>

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
  Annoy::AnnoyIndex<int, double, Annoy::Angular, Annoy::Kiss32Random, Annoy::AnnoyIndexSingleThreadedBuildPolicy> *annoy_index;
  AnnoyService();
  AnnoyService(const AnnoyService &) = delete;
  AnnoyService &operator=(const AnnoyService &) = delete;

public:
  static AnnoyService &getInstance();
  ~AnnoyService();
  double get_closest(char *error, char *vector_arg, char *is_null);
  bool load_index(char *message);
  void populate_annoy_from_db();
};

#endif // ANNOY_SERVICE_H

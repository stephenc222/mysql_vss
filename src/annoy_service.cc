#include <fstream>
#include "mysql/plugin.h"
#include "mysql.h"
#include "jansson.h"
#include "kissrandom.h"
#include "annoylib.h"
#include "include/annoy_service.h"

AnnoyService::AnnoyService() : annoy_index(nullptr) {}

AnnoyService &AnnoyService::getInstance()
{
  static AnnoyService instance;
  return instance;
}

AnnoyService::~AnnoyService()
{
  // Clean up resources if any
  if (annoy_index)
  {
    delete annoy_index;
    annoy_index = nullptr;
  }
}

double AnnoyService::get_closest(char *error, char *vector_arg, char *is_null)
{
  // Parse the JSON vector
  json_t *root;
  json_error_t error1;

  root = json_loads(vector_arg, &error1);

  if (!root)
  {
    strcpy(error, "Error parsing JSON input.");
    *is_null = 1;
    return 0.0;
  }

  // Convert JSON vector to model
  double v[EMBEDDING_DIM];

  for (int i = 0; i < EMBEDDING_DIM; i++)
  {
    if (!json_is_real(json_array_get(root, i)))
    {
      strcpy(error, "Invalid or missing values in JSON array.");
      *is_null = 1;
      json_decref(root);
      return 0.0;
    }
    v[i] = json_real_value(json_array_get(root, i));
  }

  // Clean up the parsed JSON object
  json_decref(root);

  // Use Annoy index to get the most similar vector

  std::vector<int> closest_items;
  // std::vector<double> distances; // If you want the distances, otherwise this can be omitted
  annoy_index->get_nns_by_vector(v, 1, -1, &closest_items, NULL);
  int closest_item = closest_items[0];

  return (double)closest_item; // For this example, we return the index of the most similar item
};

bool AnnoyService::load_index(char *message)
{
  if (!annoy_index)
  {
    char const *annoyIndexPath = ANNOY_INDEX_PATH;
    annoy_index = new AnnoyIndex<int, double, Angular, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy>(EMBEDDING_DIM);
    std::ifstream ifile(annoyIndexPath);

    // Try loading the index from disk
    if (!ifile || !annoy_index->load(annoyIndexPath))
    {
      strcpy(message, "Error loading Annoy index.");
      // If loading failed, populate the index from the database

      populate_annoy_from_db();

      // Build the index
      annoy_index->build(EMBEDDING_DIM);

      // Save the index to disk
      if (!annoy_index->save(annoyIndexPath))
      {
        strcpy(message, "Error saving Annoy index.");
        return 1; // Indicate error
      }
    }
  }
  return 0;
}

void AnnoyService::populate_annoy_from_db()
{
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

  // Initialize MySQL client library
  mysql_library_init(0, NULL, NULL);

  // Connect to database
  conn = mysql_init(NULL);
  if (conn == NULL)
  {
    // Handle error
    fprintf(stderr, "mysql_init() failed\n");
    return;
  }

  // TODO: refactor the database connection to either:
  // 1. use the internal MySQL API to perform a SQL query
  // 2. Read from a reliable configuration source to authenticate this connection
  // FIXME: regardless of how this query is handled, ensure that any errors are handled
  // Could be fatal to the MySQL server otherwise
  if (mysql_real_connect(conn, "localhost", "root", "password1234", "wordpress", 33060, NULL, 0) == NULL)
  {
    // Handle error
    fprintf(stderr, "mysql_real_connect() failed\n");
    mysql_close(conn);
    return;
  }

  // Execute SQL query to fetch data
  if (mysql_query(conn, "SELECT vector FROM embeddings"))
  {
    // Handle error
    fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
    mysql_close(conn);
    return;
  }

  res = mysql_store_result(conn);
  if (res == NULL)
  {
    // Handle error
    fprintf(stderr, "mysql_store_result() failed\n");
    mysql_close(conn);
    return;
  }

  int item_index = 0;
  while ((row = mysql_fetch_row(res)))
  {
    // Assuming row[0] is a JSON string of the vector
    json_t *root;
    json_error_t error;

    root = json_loads(row[0], &error);
    if (!root)
    {
      fprintf(stderr, "Error parsing JSON input: %s\n", error.text);
      continue; // Go to the next row
    }

    double item_data[EMBEDDING_DIM];
    for (int i = 0; i < EMBEDDING_DIM; i++)
    {
      if (!json_is_real(json_array_get(root, i)))
      {
        fprintf(stderr, "Invalid or missing values in JSON arrays.\n");
        break; // Break from the loop processing this embedding
      }
      item_data[i] = json_real_value(json_array_get(root, i));
    }

    annoy_index->add_item(item_index, item_data);
    item_index++;

    // Clean up the parsed JSON object
    json_decref(root);
  }

  // Cleanup
  mysql_free_result(res);
  mysql_close(conn);
  mysql_library_end();
}

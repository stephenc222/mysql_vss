#include "mysql/plugin.h"
#include "mysql.h"
#include "jansson.h"
#include "kissrandom.h"
#include "annoylib.h"
#include "include/annoy_service.h"

const int CHUNK_SIZE = 1000;
const int RESULT_LIST_SIZE = 10;

std::map<std::string, std::string> readConfig(const std::string &filename)
{
  std::ifstream file(filename);
  std::map<std::string, std::string> configMap;

  std::string line;
  while (std::getline(file, line))
  {
    std::istringstream is_line(line);
    std::string key;
    if (std::getline(is_line, key, '='))
    {
      std::string value;
      if (std::getline(is_line, value))
        configMap[key] = value;
    }
  }

  return configMap;
}

AnnoyService::AnnoyService() : annoy_index(nullptr)
{
  dbConfig = readConfig("mysql_vss_config.ini");
}

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
    annoy_index->unload();
    delete annoy_index;
    annoy_index = nullptr;
  }
}

char *AnnoyService::get_closest(char *error, char *vector_arg, char *result, unsigned long *length, char *is_null)
{
  // Parse the JSON vector
  json_t *root;
  json_error_t error1;

  root = json_loads(vector_arg, &error1);
  if (!root)
  {
    strcpy(error, "Error parsing JSON input.");
    *is_null = 1;
    return NULL;
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
      return NULL;
    }
    v[i] = json_real_value(json_array_get(root, i));
  }
  json_decref(root);

  // Use Annoy index to get the top 10 most similar vectors
  std::vector<int> closest_items;
  annoy_index->get_nns_by_vector(v, RESULT_LIST_SIZE, -1, &closest_items, NULL);

  // Initialize MySQL client library
  mysql_library_init(0, NULL, NULL);
  MYSQL *conn = mysql_init(NULL);
  if (!conn)
  {
    strcpy(error, "MySQL initialization failed.");
    *is_null = 1;
    return NULL;
  }

  const char *db_host = dbConfig["host"].c_str();
  const char *db_user = dbConfig["user"].c_str();
  const char *db_pass = dbConfig["password"].c_str();
  const char *db_name = dbConfig["name"].c_str();
  unsigned int db_port = std::stoi(dbConfig["port"]);

  if (mysql_real_connect(conn, db_host, db_user, db_pass, db_name, db_port, NULL, 0) == NULL)
  {
    strcpy(error, mysql_error(conn));
    mysql_close(conn);
    *is_null = 1;
    return NULL;
  }

  // Fetch the IDs for the closest items and pack them into the result buffer
  std::string id_list;
  for (int item_id : closest_items)
  {
    char query[256];
    snprintf(query, sizeof(query), "SELECT ID FROM embeddings WHERE annoy_index=%d", item_id);

    if (mysql_query(conn, query))
    {
      strcpy(error, mysql_error(conn));
      continue;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row)
    {
      id_list += row[0];
      id_list += ",";
    }

    mysql_free_result(res);
  }

  // Remove trailing comma
  if (!id_list.empty())
  {
    id_list.pop_back();
  }

  // Copy the ID list into the result buffer and set its length
  strncpy(result, id_list.c_str(), id_list.size());
  *length = id_list.size();

  mysql_close(conn);
  mysql_library_end();

  return result; // Indicate success
}

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
  if (!conn)
  {
    fprintf(stderr, "mysql_init() failed\n");
    return;
  }
  const char *db_host = dbConfig["host"].c_str();
  const char *db_user = dbConfig["user"].c_str();
  const char *db_pass = dbConfig["password"].c_str();
  const char *db_name = dbConfig["name"].c_str();
  unsigned int db_port = std::stoi(dbConfig["port"]);

  if (mysql_real_connect(conn, db_host, db_user, db_pass, db_name, db_port, NULL, 0) == NULL)
  {
    fprintf(stderr, "mysql_real_connect() failed\n");
    mysql_close(conn);
    return;
  }

  // Determine total number of rows
  if (mysql_query(conn, "SELECT COUNT(*) FROM embeddings"))
  {
    fprintf(stderr, "COUNT query error: %s\n", mysql_error(conn));
    mysql_close(conn);
    return;
  }

  res = mysql_store_result(conn);
  MYSQL_ROW countRow = mysql_fetch_row(res);
  int totalRows = atoi(countRow[0]);
  mysql_free_result(res);

  int totalChunks = (totalRows + CHUNK_SIZE - 1) / CHUNK_SIZE;

  for (int chunk = 0; chunk < totalChunks; chunk++)
  {
    int offset = chunk * CHUNK_SIZE;
    char selectSQL[256];
    snprintf(selectSQL, sizeof(selectSQL), "SELECT ID, vector FROM embeddings LIMIT %d OFFSET %d", CHUNK_SIZE, offset);

    if (mysql_query(conn, selectSQL))
    {
      fprintf(stderr, "SELECT error: %s\n", mysql_error(conn));
      continue;
    }

    res = mysql_use_result(conn);
    std::map<int, int> updateMap; // Map of item_index to db_id for the current chunk
    int item_index = offset;

    while ((row = mysql_fetch_row(res)))
    {
      int db_id = atoi(row[0]);
      json_t *root;
      json_error_t error;

      root = json_loads(row[1], &error);
      if (!root)
      {
        fprintf(stderr, "Error parsing JSON input: %s\n", error.text);
        continue;
      }

      double item_data[EMBEDDING_DIM];
      for (int i = 0; i < EMBEDDING_DIM; i++)
      {
        if (!json_is_real(json_array_get(root, i)))
        {
          fprintf(stderr, "Invalid or missing values in JSON arrays.\n");
          break;
        }
        item_data[i] = json_real_value(json_array_get(root, i));
      }

      annoy_index->add_item(item_index, item_data);
      updateMap[item_index] = db_id;

      item_index++;
      json_decref(root);
    }

    // Construct the batch update SQL for the current chunk
    std::string updateSQL = "UPDATE embeddings SET annoy_index = CASE ID ";
    for (const auto &pair : updateMap)
    {
      updateSQL += "WHEN " + std::to_string(pair.second) + " THEN " + std::to_string(pair.first) + " ";
    }
    updateSQL += "ELSE annoy_index END";

    // Execute the batch update for the current chunk
    if (mysql_query(conn, updateSQL.c_str()))
    {
      fprintf(stderr, "UPDATE error: %s\n", mysql_error(conn));
    }

    mysql_free_result(res);
  }

  mysql_close(conn);
  mysql_library_end();
}

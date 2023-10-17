#include <math.h>
#include <mysql/plugin.h>
#include "jansson.h"  // For JSON parsing

// TODO: how to make this variable length according to a user's embedding model
// This is the number of dimensions for the GTE-base embedding model
#define EMBEDDING_DIM 768

extern "C" {
// Helper function to compute dot product of two vectors
double dot_product(double *v1, double *v2, int len) {
  double sum = 0.0;
  for (int i = 0; i < len; i++) {
    sum += v1[i] * v2[i];
  }
  return sum;
}

// Helper function to compute magnitude of a vector
double magnitude(double *v, int len) {
  double sum = 0.0;
  for (int i = 0; i < len; i++) {
    sum += v[i] * v[i];
  }
  return sqrt(sum);
}

double vss_search(UDF_INIT *initid, UDF_ARGS *args, char *is_null,
                  char *error) {
  if (args->arg_count != 2) {
    strcpy(error, "Two string arguments (vectors) are required.");
    *is_null = 1;
    return 0.0;
  }

  // Parse the JSON vectors
  json_t *root1, *root2;
  json_error_t error1, error2;

  root1 = json_loads(args->args[0], &error1);
  root2 = json_loads(args->args[1], &error2);

  if (!root1 || !root2) {
    strcpy(error, "Error parsing JSON input.");
    *is_null = 1;
    return 0.0;
  }

  // model
  double v1[EMBEDDING_DIM];
  double v2[EMBEDDING_DIM];

  // Extract values from the parsed JSON and validate the arrays
  for (int i = 0; i < EMBEDDING_DIM; i++) {
    if (!json_is_real(json_array_get(root1, i)) ||
        !json_is_real(json_array_get(root2, i))) {
      strcpy(error, "Invalid or missing values in JSON arrays.");
      *is_null = 1;
      json_decref(root1);
      json_decref(root2);
      return 0.0;
    }
    v1[i] = json_real_value(json_array_get(root1, i));
    v2[i] = json_real_value(json_array_get(root2, i));
  }

  // Clean up the parsed JSON objects
  json_decref(root1);
  json_decref(root2);

  // Compute cosine similarity
  double dot = dot_product(v1, v2, EMBEDDING_DIM);
  double mag_v1 = magnitude(v1, EMBEDDING_DIM);
  double mag_v2 = magnitude(v2, EMBEDDING_DIM);
  double similarity = dot / (mag_v1 * mag_v2);
  return similarity;
}

bool vss_search_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
  if (args->arg_count != 2) {
    strcpy(message, "Two string arguments (vectors) are required.");
    return 1;  // Indicate error
  }
  // Initialize memory for the result string
  initid->ptr =
      (char *)malloc(50);  // allocate for string representation of double
  return (initid->ptr == NULL) ? 1 : 0;
}

void vss_search_deinit(UDF_INIT *initid) {
  if (initid->ptr) {
    free(initid->ptr);
  }
}
}

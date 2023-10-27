# MySQL VSS

MySQL plugin for storing and querying vector embeddings.

## Getting Started

TODO:

To initialize the Git submodules currently containing Annoy from Spotify, use:

```bash
git submodule update --init --recursive
```

Use the seed script to initialize our test database:

```bash
./bin/mysql -u root -p  < ../plugin/mysql_vss/seed_data.sql
```

Run to create our custom loadable functions:

```sql
CREATE FUNCTION vss_version RETURNS STRING SONAME 'mysql_vss.so';
CREATE FUNCTION vss_search RETURNS REAL SONAME 'mysql_vss.so';
```

To check if the plugin was installed correctly:

```sql
select CAST(vss_version() AS CHAR(100));
```

Should return:

```lang-none
+----------------------------------+
| CAST(vss_version() AS CHAR(100)) |
+----------------------------------+
| 1.0.0                            |
+----------------------------------+
1 row in set (0.00 sec)
```

Testing the `vss_search` loadable function:

```sql
select CAST(vss_search('[1.0, 2.0, 3.0]', '[1.0, 2.0, 4.0]') AS CHAR(100));
```

```lang-none
+---------------------------------------------------------------------+
| CAST(vss_search('[1.0, 2.0, 3.0]', '[1.0, 2.0, 4.0]') AS CHAR(100)) |
+---------------------------------------------------------------------+
| 0.991460                                                            |
+---------------------------------------------------------------------+
1 row in set (0.01 sec)
```

Another example assuming query_embedding is a JSON array representing a vector embedding:

```python
    # Use the combined SQL query to fetch post content, its embedding, and compute the
    # cosine similarity score, then sort the results
    combined_query = f"""
    SELECT
        ID,
        original_text,
        vss_search('{json.dumps(query_embedding)}', vector) AS similarity_score
    FROM
        embeddings
    ORDER BY
        similarity_score DESC;
    """
    cursor.execute(combined_query)
    sorted_results = cursor.fetchall()

```

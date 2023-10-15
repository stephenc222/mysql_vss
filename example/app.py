import mysql.connector

# Database connection parameters
db_config = {
    "host": "localhost",
    "user": "root",
    "password": "password1234",
    "database": "wordpress",
    "port": 3306
}

# SQL queries to execute
sql_queries = [
    "CREATE FUNCTION IF NOT EXISTS vss_version RETURNS STRING SONAME 'mysql_vss.so';",
    "CREATE FUNCTION IF NOT EXISTS vss_search RETURNS INT SONAME 'mysql_vss.so';",
    "SELECT CAST(vss_version() AS CHAR(100));",
    "SELECT * FROM wp_posts ORDER BY ID LIMIT 1;"
]


# Connect to the database
try:
    db_connection = mysql.connector.connect(**db_config)
    cursor = db_connection.cursor()

    # Execute the query
    for sql_query in sql_queries:
        # Execute the query
        cursor.execute(sql_query)

        # If it is a SELECT query, fetch and display the results
        if sql_query.strip().upper().startswith("SELECT"):
            record = cursor.fetchone()
            if record:
                print("Result:", record[0])
            else:
                print("No records found")

except mysql.connector.Error as err:
    print("Error:", err)

finally:
    # Close the cursor and connection
    cursor.close()
    db_connection.close()

import mysql.connector
import json
from embedding_util import generate_embeddings

# Database connection parameters
db_config = {
    "host": "localhost",
    "user": "root",
    "password": "password1234",
    "database": "wordpress",
    "port": 3306
}

# Connect to the database
try:
    db_connection = mysql.connector.connect(**db_config)
    cursor = db_connection.cursor()

    # Fetch each post content from the wp_posts table
    cursor.execute("SELECT ID, post_content FROM wp_posts;")
    records = cursor.fetchall()

    for record in records:
        post_id, post_content = record
        # Generate a vector embedding for the post content
        embedding = generate_embeddings(post_content)

        # Insert the generated embedding into the embeddings table
        insert_query = """
        INSERT INTO embeddings (ID, vector, original_text)
        VALUES (%s, %s, %s);
        """
        cursor.execute(
            insert_query, (post_id, json.dumps(embedding), post_content))

    # Commit the changes to the database
    db_connection.commit()

    query = "Do you have any content about sea creatures?"
    # Generate an embedding for the query
    query_embedding = generate_embeddings(query)

    # Get the list of closest embedding IDs
    id_query = f"SELECT CAST(vss_search('{json.dumps(query_embedding)}') as CHAR)"
    cursor.execute(id_query)
    id_result = cursor.fetchone()[0]

    # Split the result into individual IDs
    ids = id_result.split(',')

    # Look up the original content from the source embeddings table using the retrieved IDs
    combined_query = f"""
    SELECT 
        e.original_text
    FROM 
        embeddings AS e
    WHERE 
        e.ID IN ({','.join(ids)})
    ORDER BY
        FIELD(e.ID, {','.join(ids)})
    """

    cursor.execute(combined_query)
    results = cursor.fetchall()

    print(f"QUERY: {query}")
    if results:
        for idx, row in enumerate(results):
            print(f"TOP RESULT {idx + 1}: {row[0]}")
    else:
        print("No matches found.")

except mysql.connector.Error as err:
    print("Error:", err)

finally:
    # Close the cursor and connection
    cursor.close()
    db_connection.close()

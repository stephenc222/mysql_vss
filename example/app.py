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

    # Look up the original content from the source embeddings table using the query vector embedding
    combined_query = f"""
    SELECT 
        e.original_text
    FROM 
        embeddings AS e
    WHERE 
        e.ID = vss_search('{json.dumps(query_embedding)}')
    """

    cursor.execute(combined_query)
    result = cursor.fetchone()
    print(f"QUERY: {query}")
    if result:
        print(f"RESULT: {result[0]}")
    else:
        print("No match found.")


except mysql.connector.Error as err:
    print("Error:", err)

finally:
    # Close the cursor and connection
    cursor.close()
    db_connection.close()

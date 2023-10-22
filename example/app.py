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

    # Generate an embedding for the query
    query_embedding = generate_embeddings(
        "Do you have any content about sea creatures?")

    # Use the combined SQL query to fetch post content, its embedding, and compute the
    # cosine similarity score, then sort the results
    combined_query = f"""
    SELECT 
        vss_search('{json.dumps(query_embedding)}') AS matched_index
    """

    cursor.execute(combined_query)
    result = cursor.fetchone()
    print(f"Matched index from Annoy: {result[0]}")


except mysql.connector.Error as err:
    print("Error:", err)

finally:
    # Close the cursor and connection
    cursor.close()
    db_connection.close()

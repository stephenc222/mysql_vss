-- Check if the "wordpress" database exists, if not, create it
CREATE DATABASE IF NOT EXISTS wordpress;

-- Use the wordpress database
USE wordpress;

-- create our vss_search function
CREATE FUNCTION vss_search RETURNS STRING SONAME 'libmysql_vss.so';

-- our embeddings table (for our example app)
CREATE TABLE IF NOT EXISTS embeddings (
    ID INT PRIMARY KEY,
    vector JSON NOT NULL,
    original_text TEXT NOT NULL,
    annoy_index INT
    -- Any other related fields
);

-- Check if the wp_posts table exists, if not, create it
CREATE TABLE IF NOT EXISTS wp_posts (
    ID INT PRIMARY KEY,
    post_author INT,
    post_date DATETIME,
    post_content TEXT,
    post_title VARCHAR(255)
    -- Add other necessary columns as per your schema
);

-- Insert posts only if they do not exist
INSERT IGNORE INTO wp_posts (ID, post_author, post_date, post_content, post_title) VALUES 
(1, 1, '2023-01-01 00:00:00', 'The quick brown fox jumps over the lazy dog.', 'Jumping Fox'),
(2, 1, '2023-01-02 00:00:00', 'Sunsets on Mars appear blue to human observers.', 'Blue Mars Sunsets'),
(3, 1, '2023-01-03 00:00:00', 'Honey never spoils; archaeologists have found pots of honey in ancient Egyptian tombs that are over 3,000 years old and still perfectly good to eat.', 'Eternal Honey'),
(4, 1, '2023-01-04 00:00:00', 'A day on Venus is longer than a year on Venus; it takes Venus 243 Earth-days to fully rotate on its axis, but only 225 Earth-days to orbit the Sun.', 'Venusian Time'),
(5, 1, '2023-01-05 00:00:00', 'Bananas are berries, but strawberries are not.', 'Berry Confusion'),
(6, 1, '2023-01-06 00:00:00', 'Shakespeare invented the name Jessica for his play Merchant of Venice.', 'Shakespearean Name'),
(7, 1, '2023-01-07 00:00:00', 'The Eiffel Tower can be 15 cm taller during the summer when its iron structure expands due to the heat.', 'Expanding Eiffel'),
(8, 1, '2023-01-08 00:00:00', 'A group of flamingos is called a "flamboyance".', 'Flamingo Group'),
(9, 1, '2023-01-09 00:00:00', 'Octopuses have three hearts, but two of them actually stop beating when they swim.', 'Octopus Hearts'),
(10, 1, '2023-01-10 00:00:00', 'The shortest war in history was between Zanzibar and England in 1896; Zanzibar surrendered after 38 minutes.', 'Shortest War');

#!/bin/bash
set -eo pipefail

# Ensure environment variables are set
if [ -z "$MYSQL_PASSWORD" ] && [ -z "$MYSQL_ALLOW_EMPTY_PASSWORD" ] && [ -z "$MYSQL_RANDOM_ROOT_PASSWORD" ]; then
    echo "Error: You must specify one of MYSQL_PASSWORD, MYSQL_ALLOW_EMPTY_PASSWORD, or MYSQL_RANDOM_ROOT_PASSWORD"
    exit 1
fi

# Check if the database is already initialized
if [ ! -d "/var/lib/mysql/mysql" ]; then
    # Initialize database
    echo "Initializing database..."
    mysqld --initialize-insecure 

    # # Start MySQL temporarily without authentication
    echo "Starting MySQL without authentication..."
    mysqld --skip-networking &
    pid="$!"

    sleep 5

    if [ -n "$MYSQL_DATABASE" ]; then
        echo "Creating database..."
        mysql --protocol=socket -uroot -e "CREATE DATABASE IF NOT EXISTS \`$MYSQL_DATABASE\`"
    fi

    if [ -n "$MYSQL_USER" ] && [ -n "$MYSQL_PASSWORD" ] && [ -n "$MYSQL_DATABASE" ]; then
        echo "Creating user and granting permissions..."
        mysql --protocol=socket -uroot -e "CREATE USER '$MYSQL_USER'@'%' IDENTIFIED BY '$MYSQL_PASSWORD'"
        mysql --protocol=socket -uroot -e "GRANT ALL PRIVILEGES ON mysql.* TO 'mysql'@'%' WITH GRANT OPTION"
        mysql --protocol=socket -uroot -e "GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE//_/\\_}\`.* TO '$MYSQL_USER'@'%' WITH GRANT OPTION"
        mysql --protocol=socket -uroot -e "FLUSH PRIVILEGES"

    else
        echo "Warning: MYSQL_USER, MYSQL_PASSWORD, and MYSQL_DATABASE need to be set to create and grant permissions to a user."
    fi

    # Stop MySQL
    echo "Stopping MySQL..."
    kill -s TERM "$pid"
    wait "$pid"
fi

# Execute the .sql script
if [ -f "/tmp/example_init.sql" ]; then
    echo "Executing .sql script from /tmp directory..."
    mysqld --skip-networking &
    pid="$!"
    sleep 5
    mysql --protocol=socket -uroot < /tmp/example_init.sql
    echo "Stopping MySQL after executing .sql script..."
    kill -s TERM "$pid"
    wait "$pid"
else
    echo ".sql script not found in /tmp directory."
fi


echo "Copying .ini file..."
cp /tmp/mysql_vss_config.ini /var/lib/mysql/mysql_vss_config.ini


# Run MySQL
echo "Running MySQL..."
exec mysqld

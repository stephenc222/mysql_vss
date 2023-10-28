FROM amazonlinux:2023.2.20231018.2

# Set the credentials and database name
ENV MYSQL_USER=mysql
ENV MYSQL_PASSWORD=password1234
ENV MYSQL_ROOT_PASSWORD=rootpassword1234
ENV MYSQL_DATABASE=wordpress
ENV MYSQL_ALLOW_EMPTY_PASSWORD=true

RUN dnf install wget -y
RUN wget https://dev.mysql.com/get/mysql80-community-release-el9-1.noarch.rpm 

RUN dnf install mysql80-community-release-el9-1.noarch.rpm -y
RUN dnf install mysql-community-server -y
RUN dnf install -y findutils

# Create a temporary data directory
RUN mkdir -p /var/lib/mysql
RUN mkdir -p /var/lib/mysqld
RUN mkdir -p /temp-mysql-data
RUN chown -R mysql:mysql /temp-mysql-data
RUN chown -R mysql:mysql /var/lib/mysql
RUN chown -R mysql:mysql /var/run/mysqld

# Copy the custom configuration file
COPY mysql_vss_config.ini /tmp/
COPY example_init.sql /tmp/
# Copy the plugin .so file into the plugin directory
COPY libmysql_vss.so /usr/lib64/mysql/plugin/

# Expose the default MySQL port
EXPOSE 3306 33060

COPY docker-entrypoint.sh /docker-entrypoint.sh
RUN chmod +x /docker-entrypoint.sh

USER mysql

ENTRYPOINT ["/bin/sh", "/docker-entrypoint.sh"]
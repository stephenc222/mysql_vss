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

# Create a temporary data directory
RUN mkdir -p /var/lib/mysql
RUN mkdir -p /var/lib/mysqld
RUN chown -R mysql:mysql /var/lib/mysql
RUN chown -R mysql:mysql /var/run/mysqld

# Define the base URL for the GitHub release
ARG BASE_URL="https://github.com/stephenc222/mysql_vss/releases/download/v0.0.1"

# Automatically detect the architecture and download the correct file, then rename it to libmysql_vss.so
RUN ARCH=$(uname -m) && \
    if [ "$ARCH" = "x86_64" ]; then \
      FILE_URL="${BASE_URL}/libmysql_vss_v0.0.1_AmazonLinux2023_x86_64.so"; \
    elif [ "$ARCH" = "aarch64" ] || [ "$ARCH" = "arm64" ]; then \
      FILE_URL="${BASE_URL}/libmysql_vss_v0.0.1_AmazonLinux2023_arm_64.so"; \
    else \
      echo "Architecture not supported" >&2; \
      exit 1; \
    fi && \
    wget -O libmysql_vss.so "$FILE_URL" && \
    mv libmysql_vss.so /usr/lib64/mysql/plugin/

# Copy the custom configuration file
COPY mysql_vss_config.ini /tmp/
COPY example_init.sql /tmp/

# Expose the default MySQL port
EXPOSE 3306 33060

COPY docker-entrypoint.sh /docker-entrypoint.sh
RUN chmod +x /docker-entrypoint.sh

USER mysql

ENTRYPOINT ["/bin/sh", "/docker-entrypoint.sh"]
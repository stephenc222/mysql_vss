#!/bin/bash

# Assumes you have a built from source mysql server located next to this project, 
# for ease of developing this plugin
cp example.mysql_vss_config.ini ../mysql-server/build/data/mysql_vss_config.ini
cmake . && make && cp ./libmysql_vss*.dylib ../mysql-server/build/plugin_output_directory
# for on mac

# Assumes you have a built from source mysql server located next to this project, 
# for ease of developing this UDF
cp example.mysql_vss_config.ini ../mysql-server/build/data/mysql_vss_config.ini
cmake . && make && mv ./libmysql_vss.dylib ../mysql-server/build/plugin_output_directory/libmysql_vss.dylib
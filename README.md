# MySQL VSS

`mysql_vss` is a plugin designed for storing and searching vector embeddings using approximate nearest neighbor search, leveraging the [Annoy](https://github.com/spotify/annoy) library for fast lookups in high-dimensional spaces.

🚧 **Experimental Stage**: The plugin is experimental and not recommended for production use yet.

## Setup

### Prerequisites

- Install recent versions of `gcc` and `g++`.
- Ensure necessary build tools and libraries are installed. These include:
  - g++
  - gcc
  - libstdc++-static
  - cmake
  - openssl-devel
  - python-devel
  - ncurses-devel

### Building the Plugin

1. Clone the repository:

```bash
git clone https://github.com/stephenc222/mysql_vss
cd mysql_vss
```

2. Initialize and update submodules:

```bash
git submodule update --init --recursive --progress
```

3. Compile `mysql-server`. This plugin requires a `mysql-server` build from source to link against:

```bash
cd src/vendor/mysql-server
mkdir build
cd build
cmake ..
make
```

\*NOTE: `mysql_vss` uses `mysql-server` version 8.0, and this is pegged in the `mysql-server` git submodule. There will be additional packages you'll need to install, such as a modern version of `bison` and others. CMake's output is pretty helpful in determining what you need

4. Compile the `mysql-vss` plugin source:

```bash
cmake .
make
```

The compiled output is a shared library named something like `libmysql_vss_v0.0.1_AmazonLinux2023_x86_64.so`, tailored to your operating system.

### Deploying the Plugin

1. Deploy the shared library to MySQL's plugin directory.
2. Register the UDFs in MySQL:

```sql
CREATE FUNCTION vss_search RETURNS STRING SONAME 'libmysql_vss.so';
CREATE FUNCTION vss_version RETURNS STRING SONAME 'libmysql_vss.so';
```

## Usage

### Verification

Verify installation by checking the plugin version:

```sql
SELECT CAST(vss_version() AS CHAR);
```

### Schema Setup

Set up the `embeddings` table as per the required schema (currently a manual process):

```sql
CREATE TABLE IF NOT EXISTS embeddings (
    ID INT PRIMARY KEY,
    vector JSON NOT NULL,
    original_text TEXT NOT NULL,
    annoy_index INT
);
```

### Performing Searches

Use `vss_search` for querying similar embeddings:

```sql
SELECT e.original_text
FROM embeddings AS e
WHERE FIND_IN_SET(e.ID, CAST(vss_search('[0.01,0.02,0.03,...]') AS CHAR)) > 0
ORDER BY FIELD(e.ID, CAST(vss_search('[0.01,0.02,0.03,...]') AS CHAR)) DESC;
```

## Known Issues and Future Enhancements

### Embedding Dimensions

- The current version only supports 768-dimensional embeddings. `mysql_vss` was developed using the embedding model, [gte-base](https://huggingface.co/thenlper/gte-base), a top performing embedding model that is runnable on a wide variety of consumer hardware.

### Annoy Index Management

- The Annoy Index loads once and requires a manual update for new embeddings.

### Data Scaling

To enhance `mysql_vss` for larger data scales, some possible future development considerations include:

- **External Process Management**: Isolate the Annoy Index to manage memory separately from MySQL.
- **Dynamic Index Reloading**: Enable index updates without restarting the service.

Additionally, configurations that you can take as well:

- **Containerization**: Apply memory quotas to contain the Annoy Index within resource limits.
- **MySQL Optimization**: Tweak MySQL configurations for larger datasets (e.g., `innodb_buffer_pool_size`).
- **Performance Benchmarks**: Test against various data sizes to determine performance and stability.

### Testing

- Expand testing to include unit and integration tests for robust validation.

Check out `examples/app.py` and the provided Dockerfile in the repository for demonstration and containerized deployment of `mysql_vss`.

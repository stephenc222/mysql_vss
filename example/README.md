# MySQL_VSS Example App

This example app contains a Dockerfile that creates a mysql database, installs `mysql_vss` along with seeding the database with test data.

The Python `app.py` transforms the `wp_posts` content into vector embeddings to store as JSON in `embeddings`, then performs an ad-hoc query to demonstrate how search results are ranked.

To build the example mysql image:

```bash
docker build . -t example-mysql
```

To create and start a container from our image in the foreground:

```bash
docker run --name example-mysql-container -p 3306:3306 example-mysql
```

Install the Python (using Python 3.9) dependencies:

```bash
pip install -r requirements.txt
```

Finally, to run the Python app (while the docker container is running in a separate terminal):

```bash
python app.py
```

# Similarity Join

Custom functions of similarity join for PostgreSQL



## Installation

Change `PG_SOURCE_DIR`  to the path of the Postgres source code directory (e.g. `./postgresql-11.2/`), and `PG_DIR` to the path for installation(e.g. `$HOME/pgsql/`)

(This step is optional if you have installed Postgres) Then run the following command to automatically install Postgres to the destination directory (e.g. `$HOME/pgsql`).

```
bash setup.sh
```

After we can run the back end server by

```
bash run_server.sh
```

Then connect to the server and access the database by 

```
bash connect_db.sh
```



## Extending By C Function

There are three custom functions implemented for similarity join.

1. `levenshtein_distance(a, b)`: return an integer, which is the Levenshtein distance between string a and b
2. `levenshtein_distance_less_than(a, b, threshold)`: return a bool, which is true if `levenshtein_distance(a, b) < threshold`
3. `jaccard_distance(a, b)`: return a float, which is the 2-gram jaccard distance between string a and b.



### Compile And Create Functions

Simply run

```
bash compile_and_create_funcs.sh
```



### Drop Functions

The way to drop those functions can be found in `./sql/drop_funcs.sql`. That is execute `\i ./sql/drop_funcs.sql` in psql, which can be run by `bash connect_db.sh`.



### Test

Simply test by running `bash test.sh`, which will execute some sql using the three functions.









 
# Similarity Join

Extension of similarity join for PostgreSQL



## Configuration

**This is the prerequisite of all bellow!**

Change `PG_SOURCE_DIR`  to the path of the Postgres source code directory (e.g. `./postgresql-11.2/`), and `PG_DIR` to the path for installation(e.g. `$HOME/pgsql/`)



## Installation

(This step is optional if you have installed Postgres) Run the following command to automatically install Postgres to the destination directory (e.g. `$HOME/pgsql`).

```
bash setup.sh
```

After that, we can run the back end server by

```
bash run_server.sh
```

Then connect to the server and access the database by 

```
bash connect_db.sh
```



## Extending By Core

**You're highly recommended to backup the source code first!**

After modifying `config.sh`, we can run 

```
python3 patch.py
```

which will modify the source code. 

And we can unpatch it by

```
python3 patch.py -u
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

### Test

Test by running `bash test.sh`, which will execute some naive sql using the three functions.

### Drop Functions

The way to drop those functions can be found in `./sql/drop_funcs.sql`. That is execute `\i ./sql/drop_funcs.sql` in psql, which can be start by `bash connect_db.sh`.











 
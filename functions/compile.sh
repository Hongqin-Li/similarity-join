
PG_PATH=../postgresql-11.2/src/include

cc -I $PG_PATH -fPIC -c similarity_join.c
cc -shared -o similarity_join.so similarity_join.o

cp similarity_join.so /usr/local/pgsql/lib

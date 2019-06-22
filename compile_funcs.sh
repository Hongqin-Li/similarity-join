
source config.sh

# If modified this, ./create_funcs should be modified also
SO_PREFIX=similarity_join

cc -O2 -I $PG_SOURCE_DIR/src/include -fPIC -c $FUNC_DIR/similarity_join.c -o $FUNC_DIR/$SO_PREFIX.o

cc -shared -o $FUNC_DIR/$SO_PREFIX.so $FUNC_DIR/$SO_PREFIX.o

cp $FUNC_DIR/$SO_PREFIX.so $PG_DIR/lib

rm $FUNC_DIR/$SO_PREFIX.so $FUNC_DIR/$SO_PREFIX.o

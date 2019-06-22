#!/bin/bash

# It will install Postgres from source code to specific path.
# Path to source code and installation path can be changed in ./config.sh

source config.sh

cd $PG_SOURCE_DIR/

./configure --prefix=$PG_DIR/
# --prefix denotes where Postgres should be installed

make
make install

$PG_DIR/bin/initdb -D $PG_DIR/data/
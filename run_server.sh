#!/bin/bash
source config.sh

$PG_DIR/bin/postgres -D $DB_DIR

# Then connect to the server by
# bash connect_db.sh

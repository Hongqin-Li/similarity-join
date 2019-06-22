#!/bin/bash
source config.sh

$PG_DIR/bin/postgres -D $PG_DIR/data/ 

# Then connect to the server by
# ~/postgres/bin/psql -h /tmp/ postgres

#!/bin/bash

rm -rf gen
mkdir -p gen/erlang
if [[ $# -eq 1 ]]; then
    absc -erlang -d gen/erlang/ ../src/*.abs $1
else
    echo "Wrong argument count"
    exit 1
fi
cd gen/erlang
./run -p 8080 | tee ../Result.ppxml
cd ../../


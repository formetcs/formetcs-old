#!/bin/bash

rm -rf gen
mkdir -p gen/erlang
if [[ $# -eq 1 ]]; then
    pac -o gen/Scenario.abs $1
    absc -erlang -d gen/erlang/ src/*.abs gen/Scenario.abs
else
    echo "Wrong argument count"
    exit 1
fi
cd gen/erlang
./run -p 8080 | tee ../Result.ppxml
cd ../../


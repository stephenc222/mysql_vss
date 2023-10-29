#!/bin/bash

mkdir -p build
cmake . && make && cp ./libmysql_vss*.dylib ./build
#!/bin/bash
# assumes .deb package requirements:
#  protobuf-compiler python-protobuf protobuf-c-compiler libprotoc-dev
SRC_DIR=.
DST_DIR=../gen

# generate the cpp and python bindings for the addressbook proto file
protoc -I=$SRC_DIR --cpp_out=$DST_DIR/cpp --python_out=$DST_DIR/py $SRC_DIR/addressbook.proto
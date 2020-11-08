#!/bin/bash

filename='test.cpp'
output_path='google-test/gtest-printer.cpp'
ninja_arguments='test-builder'

if [ $(echo "$(ninja $ninja_arguments)" |  grep -q "error") ]
then 
	exit 1
fi

mkdir -p google-test

bin/test-builder "$filename" > "$output_path"

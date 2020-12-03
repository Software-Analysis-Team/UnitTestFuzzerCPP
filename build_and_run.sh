#!/bin/bash

set -e

filename='test.cpp'
output_path='google-test/gtest-printer.cpp'
ninja_arguments='test-builder'

ninja $ninja_arguments

mkdir -p google-test

bin/test-builder "$filename" > "$output_path"

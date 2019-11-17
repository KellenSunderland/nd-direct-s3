#!/bin/bash

# Run clang-format linting
cd /workspaces/nd-direct-s3/
clang-format-7 -i *.cpp

# Run clang-tidy static code analysis
cd /workspaces/nd-direct-s3/build
run-clang-tidy-7.py

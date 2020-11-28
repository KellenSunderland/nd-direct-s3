#!/bin/bash

# Run clang-format linting
cd /workspaces/nd-direct-s3/
clang-format-7 -i *.cpp *.h

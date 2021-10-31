#!/bin/bash
find . -regex '\.\/.*\.\(h\|c\)' -exec clang-format --verbose -style=file -i {} \; 
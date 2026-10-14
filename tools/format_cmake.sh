#!/bin/sh
for dir in cesspool cmake packages runtime; do
    find "$dir" \
         \( -name 'CMakeLists.txt' \
         -o -name '*.cmake' \) \
         -exec "cmake-format" --config-files .cmake-format -i '{}' \;
done
"cmake-format" --config-files .cmake-format -i CMakeLists.txt;

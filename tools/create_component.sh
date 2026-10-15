#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <path_where> <component_name>"
    exit 1
fi

PATH_WHERE=$(realpath "$1")
NAME="$2"
TARGET_DIR="$PATH_WHERE/$NAME"

# 1. Search for the 'cesspool' directory up the hierarchy
CESSPOOL_DIR=""
CURRENT_CHECK="$PATH_WHERE"

while [ "$CURRENT_CHECK" != "/" ]; do
    if [ "$(basename "$CURRENT_CHECK")" = "cesspool" ]; then
        CESSPOOL_DIR="$CURRENT_CHECK"
        break
    fi
    CURRENT_CHECK=$(dirname "$CURRENT_CHECK")
done

if [ -z "$CESSPOOL_DIR" ]; then
    echo "Error: 'cesspool' directory not found in the hierarchy above the provided path!"
    exit 1
fi

# Extract the relative path from cesspool to the target component directory
# Example: packages/composite_root/app
FULL_RELATIVE="${TARGET_DIR#$CESSPOOL_DIR/}"

# Strip the first directory layer (e.g., "packages/" or "runtime/")
# Example result: composite_root/app
RELATIVE_PATH_FROM_CESSPOOL="${FULL_RELATIVE#*/}"

# 2. Create directory structure
INCLUDE_FULL_PATH="$TARGET_DIR/include/$RELATIVE_PATH_FROM_CESSPOOL"

mkdir -p "$TARGET_DIR/src"
mkdir -p "$TARGET_DIR/tests"
mkdir -p "$INCLUDE_FULL_PATH"

# 3. Create completely empty CMakeLists.txt files
touch "$PATH_WHERE/CMakeLists.txt"
touch "$TARGET_DIR/src/CMakeLists.txt"
touch "$TARGET_DIR/tests/CMakeLists.txt"

echo "Directory structure and empty CMakeLists.txt files have been created."
echo "Component created at: $TARGET_DIR"

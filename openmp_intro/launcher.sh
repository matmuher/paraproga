#!/bin/bash

# Function to get app name from arguments
get_app_name() {
    case "$1" in
        --app=*|--app)
            echo "${1#--app=}"
            ;;
        "")
            echo "a.out"
            ;;
        *)
            echo "Usage: $0 [--app=<app_name>]"
            exit 1
            ;;
    esac
}

# Get app name
APP_NAME=$(get_app_name "$1")

# Check if app name was provided
if [ -z "$APP_NAME" ]; then
    echo "Warning: No app name provided. Using default 'a.out'"
else
    echo "App name: $APP_NAME"
fi

# Generate sequence 1, 3, 5, ..., 11
for i in $(seq 1 1 12); do
    OMP_NUM_THREADS=$i
    echo "$APP_NAME, OMP_NUM_THREADS=$OMP_NUM_THREADS"
    ./"$APP_NAME"
done

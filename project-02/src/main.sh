#!/bin/bash
OUTPUT_FILE="output.txt"

# Function to compile program
compile() {
    echo "Compiling readers-writers program"
    
    if gcc -o readers_writers main.c readers_writers.c -Wall -Wextra -pthread; then
        echo "Compilation successful"
        echo
        return 0
    else
        echo "Compilation failed"
        return 1
    fi
}

# Function to run the program
run() {
    echo "Running Readers-Writers Problem"
    echo
    
    ./readers_writers | tee "$OUTPUT_FILE"
    echo
    echo "Results saved to $OUTPUT_FILE"
}

# Function to clean up executables
clean() {
    echo "Cleaning up executables"
    rm -f readers_writers
    echo "Cleanup completed"
}

# Main execution
main() {
    if ! compile; then
        echo "Compilation failed. Exiting"
        exit 1
    fi
    
    run
    clean
}

# Run main function
main


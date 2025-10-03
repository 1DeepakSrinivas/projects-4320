#!/bin/bash

# Compile the programs (without .exe extension for Linux)
gcc -o round_robin round_robin.c
gcc -o sjf_1 sjf_1.c

# Create or clear the output file
output_file="output.txt"
> "$output_file"

# Check if compilation was successful
if [ ! -f "round_robin" ] || [ ! -f "sjf_1" ]; then
    echo "Compilation failed. Please check your source files." | tee -a "$output_file"
    exit 1
fi

echo "Running Round Robin Scheduler:" | tee -a "$output_file"
rr_output=$(./round_robin)
echo "$rr_output" | tee -a "$output_file"
echo | tee -a "$output_file"

echo "Running SJF Scheduler:" | tee -a "$output_file"
sjf_output=$(./sjf_1)
echo "$sjf_output" | tee -a "$output_file"
echo | tee -a "$output_file"

# Extract metrics
rr_wait_time=$(echo "$rr_output" | grep -i "waiting time" | awk '{print $NF}' | sed 's/[^0-9.]//g')
rr_turnaround_time=$(echo "$rr_output" | grep -i "turnaround time" | awk '{print $NF}' | sed 's/[^0-9.]//g')

sjf_wait_time=$(echo "$sjf_output" | grep -i "waiting time" | awk '{print $NF}' | sed 's/[^0-9.]//g')
sjf_turnaround_time=$(echo "$sjf_output" | grep -i "turnaround time" | awk '{print $NF}' | sed 's/[^0-9.]//g')

# Compare wait times
echo "Comparison of Average Wait Times:" | tee -a "$output_file"
echo "Round Robin: $rr_wait_time" | tee -a "$output_file"
echo "SJF: $sjf_wait_time" | tee -a "$output_file"
echo | tee -a "$output_file"

echo "Comparison of Average Turnaround Times:" | tee -a "$output_file"
echo "Round Robin: $rr_turnaround_time" | tee -a "$output_file"
echo "SJF: $sjf_turnaround_time" | tee -a "$output_file"
echo | tee -a "$output_file"

# Determine which scheduler has lower wait time using awk for floating-point comparison
if [ -n "$rr_wait_time" ] && [ -n "$sjf_wait_time" ]; then
    result=$(awk -v rr="$rr_wait_time" -v sjf="$sjf_wait_time" 'BEGIN {
        if (rr < sjf) print "rr";
        else if (rr > sjf) print "sjf";
        else print "equal";
    }')
    
    if [ "$result" = "rr" ]; then
        echo "Round Robin scheduler has lower average wait time." | tee -a "$output_file"
    elif [ "$result" = "sjf" ]; then
        echo "SJF scheduler has lower average wait time." | tee -a "$output_file"
    else
        echo "Both schedulers have the same average wait time." | tee -a "$output_file"
    fi
else
    echo "Error: Could not extract wait times for comparison." | tee -a "$output_file"
fi

# Clean up
rm -f round_robin sjf_1

# Display that output was saved to file
echo -e "\nOutput has been saved to $output_file"
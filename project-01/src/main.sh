#!/bin/bash
OUTPUT_FILE="output.txt"

# Function to compile programs
compile_programs() {
    echo "Compiling programs..."
    
    # Compile SJF
    if gcc -o sjf_1 sjf_1.c -Wall -Wextra; then
        echo "SJF compiled successfully"
    else
        echo "SJF compilation failed"
        return 1
    fi
    
    # Compile Round Robin
    if gcc -o round_robin round_robin.c -Wall -Wextra; then
        echo "Round Robin compiled successfully"
    else
        echo "Round Robin compilation failed"
        return 1
    fi
    
    echo
    return 0
}

# Function to check if processes.txt exists
check_processes_file() {
    if [ ! -f "processes.txt" ]; then
        echo "Error: processes.txt not found!"
        echo "Please ensure processes.txt exists in the current directory."
        return 1
    fi
    return 0
}

# Function to run SJF only
run_sjf() {
    echo "---Running Shortest Job First (SJF) Scheduler---"
    echo
    
    if ! check_processes_file; then
        return 1
    fi
    
    ./sjf_1 | tee -a "$OUTPUT_FILE"
    echo
}

# Function to run Round Robin only
run_round_robin() {
    echo "---Running Round Robin Scheduler---"
    echo
    
    if ! check_processes_file; then
        return 1
    fi
    
    ./round_robin | tee -a "$OUTPUT_FILE"
    echo
}

# Function to run comparison
run_comparison() {
    echo "---Running Algorithm Comparison---"
    echo
    
    if ! check_processes_file; then
        return 1
    fi
    
    # Clear output file
    > "$OUTPUT_FILE"
    
    echo "---Algorithm Comparison---" | tee -a "$OUTPUT_FILE"
    echo | tee -a "$OUTPUT_FILE"
    
    # Run Round Robin
    echo "Round Robin Scheduler:" | tee -a "$OUTPUT_FILE"
    rr_output=$(./round_robin)
    echo "$rr_output" | tee -a "$OUTPUT_FILE"
    echo | tee -a "$OUTPUT_FILE"
    
    # Run SJF
    echo "Shortest Job First (SJF) Scheduler:" | tee -a "$OUTPUT_FILE"
    sjf_output=$(./sjf_1)
    echo "$sjf_output" | tee -a "$OUTPUT_FILE"
    echo | tee -a "$OUTPUT_FILE"
    
    #  metrics for comparison
    echo "---Performance Comparison---" | tee -a "$OUTPUT_FILE"
    echo | tee -a "$OUTPUT_FILE"
    
    #  average waiting times
    rr_wait_time=$(echo "$rr_output" | grep -i "average waiting time" | head -1 | awk '{print $NF}')
    sjf_wait_time=$(echo "$sjf_output" | grep -i "average waiting time" | head -1 | awk '{print $NF}')
    
    # average turnaround times
    rr_turnaround_time=$(echo "$rr_output" | grep -i "average turnaround time" | head -1 | awk '{print $NF}')
    sjf_turnaround_time=$(echo "$sjf_output" | grep -i "average turnaround time" | head -1 | awk '{print $NF}')
    
    # Display comparison results
    echo "Average Waiting Times:" | tee -a "$OUTPUT_FILE"
    echo "Round Robin: $rr_wait_time" | tee -a "$OUTPUT_FILE"
    echo "SJF:         $sjf_wait_time" | tee -a "$OUTPUT_FILE"
    echo | tee -a "$OUTPUT_FILE"
    
    echo "Average Turnaround Times:" | tee -a "$OUTPUT_FILE"
    echo "Round Robin: $rr_turnaround_time" | tee -a "$OUTPUT_FILE"
    echo "SJF:         $sjf_turnaround_time" | tee -a "$OUTPUT_FILE"
    echo | tee -a "$OUTPUT_FILE"
    
    # Determine which scheduler performs better
    if [ -n "$rr_wait_time" ] && [ -n "$sjf_wait_time" ]; then
        result=$(awk -v rr="$rr_wait_time" -v sjf="$sjf_wait_time" 'BEGIN {
            if (rr < sjf) print "Round Robin has lower average wait time";
            else if (rr > sjf) print "SJF has lower average wait time";
            else print "Both have the same average wait time";
        }')
        
        echo "Conclusion: $result" | tee -a "$OUTPUT_FILE"
    else
        echo "Error: Could not extract wait times for comparison." | tee -a "$OUTPUT_FILE"
    fi
    
    echo | tee -a "$OUTPUT_FILE"
    echo "Results saved to $OUTPUT_FILE"
    echo
}

# Function to display menu
display_menu() {
    echo "Please select an option:"
    echo "1. Run Shortest Job First (SJF) Scheduler"
    echo "2. Run Round Robin Scheduler"
    echo "3. Run Algorithm Comparison"
    echo "4. View Output File"
    echo "5. Clean Up (Remove executables)"
    echo "6. Exit"
    echo
    echo -n "Enter your choice [1-6]: "
}

# Function to view output file
view_output() {
    if [ -f "$OUTPUT_FILE" ]; then
        echo "---Contents of $OUTPUT_FILE---"
        cat "$OUTPUT_FILE"
        echo
    else
        echo "Output file does not exist yet."
        echo
    fi
}

# Function to clean up
cleanup() {
    echo "Cleaning up executables..."
    rm -f sjf_1 round_robin
    echo "Cleanup completed!"
    echo
}

# Function to pause
pause() {
    echo -n "Press Enter to continue..."
    read
    clear
}

# Main script
main() {
    clear
    echo "---CPU Scheduling & Memory Management Simulator---"
    echo
    
    # Compile programs at startup
    if ! compile_programs; then
        echo "Compilation failed. Exiting..."
        exit 1
    fi
    
    while true; do
        display_menu
        read choice
        echo
        
        case $choice in
            1)
                run_sjf
                pause
                ;;
            2)
                run_round_robin
                pause
                ;;
            3)
                run_comparison
                pause
                ;;
            4)
                view_output
                pause
                ;;
            5)
                cleanup
                pause
                ;;
            6)
                echo "Thank you for using the CPU Scheduling Simulator!"
                cleanup
                exit 0
                ;;
            *)
                echo "Invalid choice. Please enter a number between 1 and 6."
                echo
                pause
                ;;
        esac
    done
}

# Run the main function
main
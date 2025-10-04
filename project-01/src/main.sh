#!/bin/bash

# Function to compile programs
compile_programs() {
    echo "Compiling programs..."
    gcc -o round_robin round_robin.c
    gcc -o sjf_1 sjf_1.c
    
    if [ ! -f "round_robin" ] || [ ! -f "sjf_1" ]; then
        echo "Compilation failed. Please check your source files."
        exit 1
    fi
    echo "Compilation successful!"
    echo
}

# Function to run SJF only
run_sjf() {
    echo "Running SJF Scheduler"
    ./sjf_1
    echo
}

# Function to run Round Robin only
run_round_robin() {
    echo "Running Round Robin Scheduler"
    ./round_robin
    echo
}

# Function to run comparison
run_comparison() {
    # Create or clear the output file
    output_file="output.txt"
    > "$output_file"
    
    echo "Running Comparison Mode"
    echo
    
    echo "Round Robin Scheduler:" | tee -a "$output_file"
    rr_output=$(./round_robin)
    echo "$rr_output" | tee -a "$output_file"
    echo | tee -a "$output_file"
    
    echo "SJF Scheduler:" | tee -a "$output_file"
    sjf_output=$(./sjf_1)
    echo "$sjf_output" | tee -a "$output_file"
    echo | tee -a "$output_file"
    
    # Extract metrics
    rr_wait_time=$(echo "$rr_output" | grep -i "waiting time" | awk '{print $NF}' | sed 's/[^0-9.]//g')
    rr_turnaround_time=$(echo "$rr_output" | grep -i "turnaround time" | awk '{print $NF}' | sed 's/[^0-9.]//g')
    
    sjf_wait_time=$(echo "$sjf_output" | grep -i "waiting time" | awk '{print $NF}' | sed 's/[^0-9.]//g')
    sjf_turnaround_time=$(echo "$sjf_output" | grep -i "turnaround time" | awk '{print $NF}' | sed 's/[^0-9.]//g')
    
    # Compare wait times
    echo "Comparison Results"
    echo
    echo "Average Wait Times:" | tee -a "$output_file"
    echo "Round Robin: $rr_wait_time" | tee -a "$output_file"
    echo "SJF: $sjf_wait_time" | tee -a "$output_file"
    echo | tee -a "$output_file"
    
    echo "Average Turnaround Times:" | tee -a "$output_file"
    echo "Round Robin: $rr_turnaround_time" | tee -a "$output_file"
    echo "SJF: $sjf_turnaround_time" | tee -a "$output_file"
    echo | tee -a "$output_file"
    
    # Determine which scheduler has lower wait time
    if [ -n "$rr_wait_time" ] && [ -n "$sjf_wait_time" ]; then
        result=$(awk -v rr="$rr_wait_time" -v sjf="$sjf_wait_time" 'BEGIN {
            if (rr < sjf) print "rr";
            else if (rr > sjf) print "sjf";
            else print "equal";
        }')
        
        if [ "$result" = "rr" ]; then
            echo "Round Robin has lower average wait time." | tee -a "$output_file"
        elif [ "$result" = "sjf" ]; then
            echo "SJF has lower average wait time." | tee -a "$output_file"
        else
            echo "Both have the same average wait time." | tee -a "$output_file"
        fi
    else
        echo "Error: Could not extract wait times for comparison." | tee -a "$output_file"
    fi
    
    echo
    echo "Output saved to $output_file"
    echo
}

# Function to display menu
display_menu() {
    echo "Process Scheduling Simulator"
    echo "----------------------------"
    echo "1. Run SJF Scheduler"
    echo "2. Run Round Robin Scheduler"
    echo "3. Run Comparison (Both Schedulers)"
    echo "4. Exit"
    echo -n "Enter your choice [1-4]: "
}

# Main script
main() {
    # Compile programs at startup
    compile_programs
    
    while true; do
        display_menu
        read choice
        echo
        
        case $choice in
            1)
                run_sjf
                ;;
            2)
                run_round_robin
                ;;
            3)
                run_comparison
                ;;
            4)
                rm -f round_robin sjf_1
                exit 0
                ;;
            *)
                echo "Invalid choice. Please enter a number between 1 and 4."
                echo
                ;;
        esac
        
        # Pause before showing menu again
        echo -n "Press Enter to continue"
        read
        clear
    done
}

# Run the main function
main

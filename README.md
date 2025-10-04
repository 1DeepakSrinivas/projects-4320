# Operating Systems Process Scheduling Simulator

This project implements and compares different CPU scheduling algorithms for CSC 4320 Operating Systems course. It provides a comprehensive simulation environment to analyze the performance characteristics of Shortest Job First (SJF) and Round Robin (RR) scheduling algorithms.

## Project Description

This process scheduling simulator demonstrates fundamental CPU scheduling concepts by implementing two key algorithms:

- **Shortest Job First (SJF)**: A non-preemptive algorithm that selects the process with the shortest burst time
- **Round Robin (RR)**: A preemptive algorithm that assigns fixed time quanta to processes in a circular queue

The simulator reads process data from a file, executes both scheduling algorithms, calculates performance metrics, and provides visual representations through Gantt charts.

## Functionality

### Core Features

1. **Process Scheduling Simulation**

   - Implements SJF and Round Robin algorithms
   - Calculates Waiting Time (WT) and Turnaround Time (TAT) for each process
   - Displays execution order through Gantt charts

2. **Performance Analysis**

   - Computes average waiting and turnaround times
   - Provides comparative analysis between algorithms
   - Outputs detailed process statistics

3. **Interactive Menu System**
   - Run individual schedulers
   - Execute comparison mode
   - Clean interface with clear options

### Algorithm Implementations

**Shortest Job First (SJF)**

- Non-preemptive scheduling
- Sorts processes by burst time
- Minimizes average waiting time for given process set

**Round Robin (RR)**

- Preemptive scheduling with time quantum = 3
- Maintains ready queue for fair CPU allocation
- Handles process arrivals during execution

## Directory Structure

```
projects-4320/
├── README.md
├── project-01/
│   └── src/
│       ├── main.sh                  # Main interaction script
│       ├── processes.txt            # Input data file
│       ├── sjf_1.c                  # SJF scheduler implementation
│       ├── round_robin.c            # Round Robin scheduler implementation
│       └── <output.txt>             # Generated comparison results
```

## Main.sh - Your Primary Interface

The `main.sh` script serves as the central control point for the entire simulation system. It provides an interactive menu-driven interface with the following capabilities:

### Features of main.sh

1. **Automatic Compilation**: Compiles both schedulers at startup using gcc
2. **Interactive Menu System**:

   - Option 1: Run SJF Scheduler only
   - Option 2: Run Round Robin Scheduler only
   - Option 3: Run Both & Compare Performance
   - Option 4: Exit and cleanup

3. **Comparison Mode**: Automatically runs both algorithms and provides:

   - Side-by-side performance metrics
   - Determines which algorithm performs better
   - Saves detailed results to `output.txt`

4. **Cleanup Management**: Removes compiled executables on exit

### Menu Options Explained

- **1. SJF Only**: Executes the Shortest Job First algorithm and displays results
- **2. Round Robin Only**: Executes the Round Robin algorithm with Gantt chart
- **3. Comparison Mode**: Runs both algorithms, extracts metrics, compares performance, and saves comprehensive results
- **4. Exit**: Performs cleanup and terminates the program

## How to Run

### Prerequisites

- Linux environment with bash shell (or WSL on Windows)
- GCC compiler installed
- Standard Unix utilities (awk, sed, grep)

### Execution Steps

1. **Navigate to the source directory**:

   ```bash
   cd project-01/src
   ```

2. **Make the script executable**:

   ```bash
   chmod +x main.sh
   ```

3. **Run the main script**:

   ```bash
   ./main.sh
   ```

4. **Follow the interactive menu**:
   - Choose option 1, 2, or 3 based on your needs
   - Option 3 (Comparison) is recommended for full analysis
   - Use option 4 to exit when finished

### Input File Format

The `processes.txt` file contains process information in the following format:

```
PID  Arrival_Time  Burst_Time  Priority
1    0            5           2
2    2            3           1
...
```

### Sample Output

When running comparison mode, you'll see:

- Individual algorithm outputs with Gantt charts
- Performance metrics comparison
- Results saved to `output.txt` for reference

### For Windows Users

If using Windows, run the script through WSL:

```bash
wsl
cd /mnt/c/path/to/project-01/src
./main.sh
```

## Expected Results

The simulation demonstrates that:

- **SJF** typically provides lower average waiting times
- **Round Robin** ensures fairness through time sharing
- **Gantt charts** clearly show execution patterns and preemption

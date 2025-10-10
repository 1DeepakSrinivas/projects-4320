# CPU Scheduling & Memory Management Simulator

This project implements CPU scheduling algorithms with comprehensive memory management for CSC 4320 Operating Systems. It provides a complete simulation environment to analyze Shortest Job First (SJF) and Round Robin (RR) scheduling algorithms alongside memory allocation and paging systems.

## Project Overview

The simulator demonstrates fundamental operating system concepts through two key components:

**CPU Scheduling Algorithms:**
- **Shortest Job First (SJF)**: Non-preemptive algorithm selecting processes with shortest burst time
- **Round Robin (RR)**: Preemptive algorithm with time quantum = 3, using circular queue

**Memory Management Systems:**
- **Contiguous Allocation**: First-fit, Best-fit, and Worst-fit algorithms
- **Paging System**: FIFO and LRU page replacement algorithms
- **Memory Simulation**: Realistic memory access patterns during process execution

## Architecture & Design

### Modular Structure

The codebase follows a clean, modular design:

```
project-01/src/
├── sjf.h                    # SJF algorithm header with macros and declarations
├── sjf_1.c                  # SJF implementation with memory management
├── round_robin.h            # Round Robin header with macros and declarations  
├── round_robin.c            # Round Robin implementation with memory management
├── main.sh                  # Interactive menu system and comparison tool
├── processes.txt            # Input process data
└── output.txt               # Generated comparison results
```

### Key Features

1. **CPU Scheduling Simulation**
   - Implements both SJF and Round Robin algorithms
   - Calculates waiting time and turnaround time for each process
   - Generates Gantt charts showing execution order
   - Handles process arrivals and preemption correctly

2. **Memory Management Integration**
   - **Contiguous Memory**: Simulates first-fit, best-fit, and worst-fit allocation
   - **Paging System**: Implements FIFO and LRU page replacement
   - **Memory Access**: Simulates realistic memory usage during process execution
   - **Status Reporting**: Detailed memory allocation and page frame information

3. **Interactive Interface**
   - Clean, simple menu system without colors or complex formatting
   - Individual algorithm execution
   - Comprehensive comparison mode with performance analysis
   - Output file management and viewing

## How to Run

### Prerequisites
- Linux environment with bash shell (or WSL on Windows)
- GCC compiler
- Standard Unix utilities (awk, grep, head)

### Execution Steps

1. **Navigate to source directory:**
   ```bash
   cd project-01/src
   ```

2. **Make script executable:**
   ```bash
   chmod +x main.sh
   ```

3. **Run the simulator:**
   ```bash
   ./main.sh
   ```

4. **Use the interactive menu:**
   - **Option 1**: Run SJF Scheduler only
   - **Option 2**: Run Round Robin Scheduler only  
   - **Option 3**: Run Algorithm Comparison (recommended)
   - **Option 4**: View Output File
   - **Option 5**: Clean Up executables
   - **Option 6**: Exit

### Input Format

The `processes.txt` file contains process information:
```
PID  Arrival_Time  Burst_Time  Priority
1    0            5           2
2    2            3           1
...
```

## Algorithm Details

### Shortest Job First (SJF)
- **Type**: Non-preemptive
- **Selection**: Shortest burst time among ready processes
- **Tie-breaking**: Earlier arrival time preferred
- **Memory**: Tests First-Fit/FIFO and Best-Fit/LRU combinations

### Round Robin (RR)
- **Type**: Preemptive with time quantum = 3
- **Queue**: Circular ready queue with fair CPU allocation
- **Preemption**: Processes re-queued if not completed in quantum
- **Memory**: Dynamic allocation when process first runs

## Memory Management Features

### Contiguous Allocation
- **First-Fit**: Allocates first suitable block found
- **Best-Fit**: Allocates smallest suitable block
- **Worst-Fit**: Allocates largest suitable block
- **Fragmentation**: Handles block splitting and merging

### Paging System
- **Page Size**: 64 bytes with 16 total page frames
- **FIFO**: First-in-first-out page replacement
- **LRU**: Least recently used page replacement
- **Access Tracking**: Updates page access times during execution

## Expected Results

The simulator demonstrates:
- **SJF Performance**: Lower average waiting times (typically ~7.20)
- **Round Robin Fairness**: Higher waiting times (~12.10) but guaranteed fairness
- **Memory Efficiency**: Different allocation strategies show varying fragmentation
- **Paging Behavior**: Page replacement algorithms handle memory pressure

## Output Features

### Gantt Charts
Visual representation of process execution order showing:
- Process IDs and execution intervals
- Preemption points for Round Robin
- Clear timeline with start/end times

### Performance Metrics
- Individual process statistics (waiting time, turnaround time)
- Average waiting and turnaround times
- Algorithm comparison with performance winner
- Memory allocation status and page frame usage

### File Output
Complete results saved to `output.txt` including:
- Both algorithm outputs
- Performance comparison
- Memory management status
- Conclusion about which algorithm performs better

## Code Quality

- **Warning-Free**: Compiles without warnings using `-Wall -Wextra`
- **Modular Design**: Clean separation of headers and implementations
- **Readable Code**: Human-like variable names and minimal, clear comments
- **Undergraduate Level**: Appropriate complexity for CS students
- **Consistent Style**: Uniform formatting using "---message---" pattern

This simulator provides a comprehensive learning tool for understanding both CPU scheduling and memory management concepts in operating systems.
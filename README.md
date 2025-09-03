# Parallel Computing Performance Comparison

This project implements and compares three different approaches to parallel processing:
- **Serial execution**: Single-threaded baseline implementation
- **Mutex-based**: Multi-threaded with mutex synchronization
- **RWLock-based**: Multi-threaded with read-write lock synchronization

## Prerequisites

- GCC compiler with C11 support
- Make utility
- POSIX-compliant system (Linux, macOS, or WSL on Windows)

## Project Structure

```
├── main.c          # Main program file with argument parsing and execution logic
├── rw_lock.c       # Read-write lock implementation
├── mutex.c         # Mutex-based synchronization implementation
├── serial.c        # Serial (single-threaded) implementation
├── linkedlist.c    # Linked list data structure implementation
└── README.md       # This file
```

## Compilation

### Compile All Versions

Open a terminal in the project directory and run:

```bash
make all
```

This will generate three executables:
- `serial_exec`
- `mutex_exec`
- `rwlock_exec`

### Compile Individual Versions

You can also compile specific versions:

```bash
make serial_exec   # Compile only the serial version
make mutex_exec    # Compile only the mutex version
make rwlock_exec   # Compile only the RWLock version
```

### Clean Up

To remove compiled files:

```bash
make clean
```

## Running the Program

### Option 1: Run All Versions Sequentially

```bash
make run_all
```

This comprehensive test will:
1. Run the serial version
2. Run the mutex version with thread counts: 1, 2, 4, 8
3. Run the RWLock version with thread counts: 1, 2, 4, 8
4. Display progress information
5. Save performance results to `performance_results_all_test.csv`

### Option 2: Run Individual Versions

Each executable accepts a single command-line argument specifying the program type:

```bash
./serial_exec 0      # 0 = Serial execution
./mutex_exec 1       # 1 = Mutex-based parallel execution
./rwlock_exec 2      # 2 = RWLock-based parallel execution
```

**Important**: The argument must match the executable version:
- `0` for serial version
- `1` for mutex version  
- `2` for RWLock version

## Performance Analysis

After running `make run_all`, check the generated `performance_results_all_test.csv` file for detailed performance metrics including:
- Execution times for each version
- Thread count variations
- Performance comparisons between different synchronization approaches

## Troubleshooting

### Compilation Issues
- Ensure GCC is installed and supports C11
- Check that all required source files are present
- Verify make utility is available

### Runtime Issues
- Confirm the correct argument is passed to each executable
- Check file permissions for executables
- Ensure sufficient system resources for multi-threaded execution

## Expected Behavior

- **Serial version**: Single-threaded baseline performance
- **Mutex version**: Performance may vary with thread count due to synchronization overhead
- **RWLock version**: Generally better performance for read-heavy workloads compared to mutex

The performance comparison will help identify the most efficient approach for your specific use case.

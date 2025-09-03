# C Program with Synchronization Mechanisms

This project implements a C program that demonstrates different synchronization mechanisms for concurrent operations on linked lists. The program supports three different execution modes: read-write locks, mutexes, and serial execution.

## Project Structure

```
.
├── main.c          # Main program file with argument parsing and execution logic
├── rw_lock.c       # Read-write lock implementation
├── mutex.c         # Mutex-based synchronization implementation
├── serial.c        # Serial (single-threaded) implementation
├── linkedlist.c    # Linked list data structure implementation
└── README.md       # This file
```

## Features

- **Multiple Synchronization Mechanisms**: Compare performance between read-write locks, mutexes, and serial execution
- **Linked List Operations**: Thread-safe implementations of common linked list operations
- **Performance Comparison**: Easy switching between different synchronization approaches
- **Thread-Safe**: Proper synchronization for concurrent access to shared data structures

## Prerequisites

- GCC compiler
- POSIX threads library (pthread)
- Math library (libm)
- Unix-like operating system (Linux, macOS, etc.)

## Compilation

The program can be compiled in three different modes:

### 1. Read-Write Lock Version
```bash
gcc main.c rw_lock.c linkedlist.c -o main -lpthread -lm
```

### 2. Mutex Version
```bash
gcc main.c mutex.c linkedlist.c -o main -lpthread -lm
```

### 3. Serial Version
```bash
gcc main.c serial.c linkedlist.c -o main
```

## Usage

After compilation, run the program with:

```bash
./main 0    # Run with mode 0
./main 1    # Run with mode 1  
./main 2    # Run with mode 2
```

### Command Line Arguments

The program accepts a single integer argument that determines the execution mode:

- **Mode 0**: Serial program
- **Mode 1**: Parallel program (based on Pthreads) with one mutex for the entire linked list
- **Mode 2**: Parallel program (based on Pthreads) with read-write locks for the entire linked list

## Implementation Details

### Read-Write Locks (`rw_lock.c`)
- Uses POSIX read-write locks (`pthread_rwlock_t`)
- Allows multiple concurrent readers
- Exclusive access for writers
- Optimal for read-heavy workloads

### Mutex Locks (`mutex.c`)
- Uses POSIX mutexes (`pthread_mutex_t`)
- Exclusive access for all operations
- Simple and reliable synchronization
- Suitable for general-purpose thread safety

### Serial Execution (`serial.c`)
- Single-threaded implementation
- No synchronization overhead
- Baseline for performance comparison
- Useful for debugging and verification

### Linked List (`linkedlist.c`)
- Thread-safe linked list implementation
- Common operations (insert, delete, search, traverse)
- Memory management handled properly
- Compatible with all synchronization mechanisms

## Performance Considerations

1. **Read-Write Locks**: Best for scenarios with many readers and few writers
2. **Mutexes**: Good general-purpose choice with predictable performance
3. **Serial**: Fastest for single-threaded scenarios, baseline for comparison

## Building and Testing

### Quick Build Script
You can create a simple build script to compile all versions:

```bash
#!/bin/bash
echo "Building read-write lock version..."
gcc main.c rw_lock.c linkedlist.c -o main_rw -lpthread -lm

echo "Building mutex version..."
gcc main.c mutex.c linkedlist.c -o main_mutex -lpthread -lm

echo "Building serial version..."
gcc main.c serial.c linkedlist.c -o main_serial

echo "All versions built successfully!"
```

### Testing All Versions
```bash
# Test read-write lock version
./main_rw 0
./main_rw 1
./main_rw 2

# Test mutex version  
./main_mutex 0
./main_mutex 1
./main_mutex 2

# Test serial version
./main_serial 0
./main_serial 1
./main_serial 2
```

## Common Issues and Troubleshooting

### Compilation Errors
- **Missing pthread**: Ensure `-lpthread` flag is included
- **Missing math library**: Ensure `-lm` flag is included for mathematical operations
- **Header files**: Make sure all necessary header files are included in source files

### Runtime Issues
- **Segmentation fault**: Check for proper memory allocation and deallocation
- **Deadlocks**: Verify proper lock ordering and release patterns
- **Race conditions**: Ensure all shared data access is properly synchronized

### Performance Issues
- **Slow execution**: Consider the overhead of synchronization mechanisms
- **Poor scalability**: Read-write locks may be better for read-heavy workloads
- **Memory leaks**: Use tools like Valgrind to detect memory issues

## Development Tips

1. **Using Makefile**: Always use `make clean` before rebuilding if you've made significant changes
2. **Debugging**: Add `-g` flag to Makefile for debugging symbols
3. **Optimization**: The Makefile already includes `-O2` for optimized builds
4. **Warnings**: The Makefile includes `-Wall` to catch potential issues
5. **Memory Checking**: Use Valgrind to detect memory leaks: `valgrind ./mutex_exec 1`
6. **Performance Testing**: Use `time` command to measure execution time of each version

## Example Extended Makefile Targets

You can add these targets to your Makefile for additional functionality:

```makefile
# Debug builds
debug: CFLAGS += -g -DDEBUG
debug: all

# Memory debugging
valgrind: all
	valgrind --leak-check=full ./serial_exec 0
	valgrind --leak-check=full ./mutex_exec 1  
	valgrind --leak-check=full ./rwlock_exec 2

# Performance testing
benchmark: all
	@echo "Benchmarking all versions..."
	@echo "Serial version:"
	@time ./serial_exec 0
	@echo "Mutex version:"
	@time ./mutex_exec 1
	@echo "RWLock version:"
	@time ./rwlock_exec 2
```



[Add author information]


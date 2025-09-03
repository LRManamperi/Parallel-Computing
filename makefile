# Makefile

all: serial_exec mutex_exec rwlock_exec

# Serial version
serial_exec: main.c linkedlist.c serial.c
	gcc -pthread -Wall -O2 main.c linkedlist.c serial.c -o serial_exec -lm

# Mutex version
mutex_exec: main.c linkedlist.c mutex.c
	gcc -pthread -Wall -O2 main.c linkedlist.c mutex.c -o mutex_exec -lm

# RWLock version
rwlock_exec: main.c linkedlist.c rw_lock.c
	gcc -pthread -Wall -O2 main.c linkedlist.c rw_lock.c -o rwlock_exec -lm

run_all: serial_exec mutex_exec rwlock_exec
	@echo "Running serial version..."
	./serial_exec 0
	@echo "Running mutex version..."
	./mutex_exec 1
	@echo "Running rwlock version..."
	./rwlock_exec 2
	@echo "All tests finished!"

clean:
	rm -f serial_exec mutex_exec rwlock_exec *.o

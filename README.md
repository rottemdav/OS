Operating Systems Projects
This repository contains three major projects developed as part of the Operating Systems Structure course. Each project explores fundamental OS concepts, including process management, parallelism, and memory allocation.

1. SMASH (Simple Shell)
A custom shell implementation that supports:

Internal commands (built-in functionalities similar to common Linux shell commands).
Execution of external commands by interfacing with the Linux operating system.
Learning Objectives:
Process creation and management using fork and exec.
Handling signals (e.g., SIGINT, SIGCHLD) for job control.
Implementing job handling features like background execution and process suspension.
2. PARALLEL_BANK (Multithreaded Bank System)
A parallel banking system that processes transactions from multiple ATMs. The program reads input files (ATM_{ATM_NUM}.txt), where each ATM has a set of commands to execute concurrently.

Learning Objectives:
Implementing thread synchronization using mutexes to avoid race conditions.
Managing shared resources safely in a multithreaded environment.
Optimizing parallel execution for efficiency.
3. MEM_ALL (Custom Memory Allocator)
A low-level memory allocator that serves as an alternative to the standard malloc family of functions.

Learning Objectives:
Understanding heap memory management and dynamic allocation.
Implementing custom allocation strategies for efficient memory usage.
Handling memory fragmentation and implementing best-fit/worst-fit allocation techniques.
Each project is structured to demonstrate key operating system principles, with well-documented code to help users understand the implementation.

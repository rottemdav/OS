# 🚀 Operating Systems Projects

This repository contains three major projects developed as part of the **Operating Systems Structure** course. Each project explores fundamental OS concepts, including process management, parallelism, and memory allocation.

---

## 📌 Table of Contents
- [SMASH - Custom Shell](#-smash-custom-shell)
- [PARALLEL_BANK - Multithreaded Bank System](#-parallel_bank-multithreaded-bank-system)
- [MEM_ALL - Custom Memory Allocator](#-mem_all-custom-memory-allocator)
- [📦 Installation & Usage](#-installation--usage)
- [📜 License](#-license)

---

## 🖥️ **SMASH - Custom Shell**
A lightweight Linux shell that supports:
- 🛠️ **Internal Commands** (Built-in shell features)
- 🔄 **Execution of External Commands**  
- 🎯 **Process Management** (Foreground & Background jobs)
- 🚨 **Signal Handling** (e.g., `SIGINT`, `SIGCHLD`)

### 📚 Learning Objectives:
✅ Process creation and management using `fork` and `exec`  
✅ Implementing job control features  
✅ Handling Unix signals  

---

## 💰 **PARALLEL_BANK - Multithreaded Bank System**
A concurrent banking system that processes transactions from multiple ATMs.  
The program reads input files (`ATM_{ATM_NUM}.txt`), where each ATM executes a set of commands **in parallel**.

### 📚 Learning Objectives:
✅ Implement **thread synchronization** using **mutexes**  
✅ Prevent **race conditions** in a multithreaded environment  
✅ Optimize **parallel execution** for efficiency  

---

## 🔢 **MEM_ALL - Custom Memory Allocator**
A low-level memory allocator that serves as an alternative to the `malloc` family of functions.

### 📚 Learning Objectives:
✅ Manage **heap memory** and dynamic allocation  
✅ Implement **efficient memory usage strategies**  
✅ Address **memory fragmentation**  

---

## 📦 **Installation & Usage**
### 🔧 Compilation
```bash
gcc -o smash smash.c -Wall -Wextra
./smash

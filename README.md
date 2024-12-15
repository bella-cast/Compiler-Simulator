# Compiler Simulator

### Overview
This project is a simulation of a **compiler's key processes**, including scanning, parsing, and converting high-level code into machine-level assembly instructions. Developed in **C++**, the program focuses on core compiler design concepts such as **symbol table management**, **bitwise operations**, and **dynamic memory handling**.

This project demonstrates my understanding of low-level programming, runtime support, and optimization techniques in compiler design as part of CS241 coursework.

---

## Features
- **Lexical Scanning and Parsing**: Simulates the first stages of compilation by scanning and parsing high-level instructions.
- **Symbol Table Management**: Implements symbol tables to handle variables, labels, and branches efficiently.
- **Bitwise Operations**: Performs logical and arithmetic operations to assemble machine code instructions.
- **Runtime Support**: Supports dynamic memory management, enabling correct memory allocation during execution.
- **Conditional Branching and Loops**: Processes control flow statements like loops, labels, and conditional branches.

---

## Technologies Used
- **Programming Language**: C++
- **Concepts Applied**: Bitwise operations, symbol tables, parsing, memory management

---
## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/bella-cast/Compiler-Simulator.git
   cd Compiler-Simulator
   ```
2. Ensure you have a C++ compiler installed (e.g., GCC).
3. Compile the code:
   ```bash
   g++ -o compiler main.cpp
   ```
4. Run the program:
   ```bash
   ./compiler
   ```

---

## How It Works
1. The program reads input instructions simulating high-level code.
2. It scans and parses the input, breaking it into tokens and identifying control flow structures (e.g., branches, loops).
3. The compiler generates intermediate assembly-like instructions using bitwise operations.
4. Symbol tables are used to manage variables, labels, and runtime memory allocation.
5. Outputs final machine-level instructions for execution.

---

# FIT VUT IOS Project 2

## Synchronization, Skibus problem

Author: Michal Pavlíček xpavlim00
<br>

## Description

This program solves the synchronization problem described in "zadani-2024.pdf", which is based on "Downey, Allen B. Think OS: A Brief Introduction to Operating Systems, Chapter 11. Green Tea Press, 2014.

## Usage

```
make
./proj2 <num skiers> <num bus stops> <bus capacity> <breakfast> <time between stops>
```

Example execution:

```
./proj2 10 5 10 1000 1000
```

Upon successful execution, a file proj2.out is created with the program's output.

## Tests

A simple Python program was implemented to test basic functionality. It tests the created output file.
<br>
Execution:

```
python -u test.py
```

Tests are described inside the file.

## File structure

The program is split into multiple C files to ensure readability. The main program lies in the file proj2.c and proj.h. The code for processes is inside actors.c/h. CLI functionality is found inside cli.c/h. Semaphore creation, deletion, and helper functions are found in semaphors.c/h. Shared variables are declared in shared.c/h.

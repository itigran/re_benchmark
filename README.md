Regular Expression Benchmarking Program
===

Goal
==

The goal of the program is to illustrate performance analysis of different Regular Expression implementations.


Organization
==

Prerequisites
==
- C++ complier such as g++
- For detailed information valgrind and kcachegrind graphviz are also required.
    To install on Ubuntu systems run the following command:
    `sudo apt install valgrind kcachegrind graphviz` 

Setup
==

All necessary targets are in Makefile:
- building
- running
- callstack 
- reporting

Running
==

Results
==

Using callgraph it is possible to dive into the performance of variaous RE implementations.
In order to see performance data run the following taget:
`make callgraph`

This will generate a callgrind.out.XXXX file. Using kcachegrind it is possible to see detailed view of all the calls
and time associated with each.

Example:
![alt text](https://raw.githubusercontent.com/itigran/re_benchmark/master/doc/callgraph.png?token=ABISCNODJO5PLILXN4DERXK42T6AW "Callgraph")


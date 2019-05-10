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

Call Graph
=

Using callgraph it is possible to dive into the performance of variaous RE implementations.
In order to see performance data run the following taget:
`make callgraph`

This will generate a callgrind.out.XXXX file. Using kcachegrind it is possible to see detailed view of all the calls
and time associated with each.

Example:
![alt text](https://raw.githubusercontent.com/itigran/re_benchmark/master/doc/callgraph.png?token=ABISCNODJO5PLILXN4DERXK42T6AW "Callgraph")


Memory Leaks
=
In order to investigate memory leaks run 
```make memleaks```
This will run the program through valgrind and produce memory leak report similar to this:
>       ==18296== Memcheck, a memory error detector
>       ==18296== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
>       ==18296== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
>       ==18296== Command: ./re_benchmark ./re_benchmark --regex-file tests/simple.txt --text-file re_benchmark.cc --test-count 100
>       ==18296== 
>       Running benchmark with:
>               count      : 100
>               regex      : 3
>               text       : 0(KB), 17 lines
>               regex runs : 51
>       STRSTR    : 0.002529 sec.
>       ==18296== Conditional jump or move depends on uninitialised value(s)
>       ==18296==    at 0x4E5EE87: pcre_exec (in /lib/x86_64-linux-gnu/libpcre.so.3.13.2)
>       ==18296==    by 0x40187E: run_pcre_tests(unsigned long, char const**, char const**) (re_benchmark.cc:189)
>       ==18296==    by 0x4020C6: main (re_benchmark.cc:378)
>       ==18296== 
>       ==18296== Conditional jump or move depends on uninitialised value(s)
>       ==18296==    at 0x4E5EFCB: pcre_exec (in /lib/x86_64-linux-gnu/libpcre.so.3.13.2)
>       ==18296==    by 0x40187E: run_pcre_tests(unsigned long, char const**, char const**) (re_benchmark.cc:189)
>       ==18296==    by 0x4020C6: main (re_benchmark.cc:378)
>       ==18296== 
>       ==18296== Conditional jump or move depends on uninitialised value(s)
>       ==18296==    at 0x4E5F768: pcre_exec (in /lib/x86_64-linux-gnu/libpcre.so.3.13.2)
>       ==18296==    by 0x40187E: run_pcre_tests(unsigned long, char const**, char const**) (re_benchmark.cc:189)
>       ==18296==    by 0x4020C6: main (re_benchmark.cc:378)
>       ==18296== 
>       PCRE      : 0.145209 sec.
>       GLIBC     : 0.991589 sec.
>       RE2       : 1.918729 sec.
>       ==18296== 
>       ==18296== HEAP SUMMARY:
>       ==18296==     in use at exit: 26,977,016 bytes in 212,512 blocks
>       ==18296==   total heap usage: 624,015 allocs, 411,503 frees, 93,116,232 bytes allocated
>       ==18296== 
>       ==18296== 4 bytes in 1 blocks are possibly lost in loss record 2 of 76
>       ==18296==    at 0x4C2DB8F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
>       ==18296==    by 0x599F4C8: re_node_set_init_copy (regex_internal.c:1031)
>       ==18296==    by 0x599F4C8: create_cd_newstate (regex_internal.c:1673)
>       ==18296==    by 0x599F4C8: re_acquire_state_context (regex_internal.c:1545)
>       ==18296==    by 0x59A4877: create_initial_state (regcomp.c:1038)
>       ==18296==    by 0x59A4877: re_compile_internal (regcomp.c:802)
>       ==18296==    by 0x59AB27E: regcomp (regcomp.c:498)
>       ==18296==    by 0x40121E: run_glibc_tests(unsigned long, char const**, char const**) (re_benchmark.cc:94)
>       ==18296==    by 0x402137: main (re_benchmark.cc:386)
>       ==18296== 
>       ==18296== 4 bytes in 1 blocks are possibly lost in loss record 3 of 76
>       ==18296==    at 0x4C2DB8F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
>       ==18296==    by 0x599F65A: re_node_set_alloc (regex_internal.c:971)
>       ==18296==    by 0x599F65A: register_state (regex_internal.c:1566)
>       ==18296==    by 0x599F65A: create_cd_newstate (regex_internal.c:1725)
>       ==18296==    by 0x599F65A: re_acquire_state_context (regex_internal.c:1545)
>       ==18296==    by 0x59A4877: create_initial_state (regcomp.c:1038)
>       ==18296==    by 0x59A4877: re_compile_internal (regcomp.c:802)
>       ==18296==    by 0x59AB27E: regcomp (regcomp.c:498)
>       ==18296==    by 0x40121E: run_glibc_tests(unsigned long, char const**, char const**) (re_benchmark.cc:94)
>       ==18296==    by 0x402137: main (re_benchmark.cc:386)
>       ==18296== 



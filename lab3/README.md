# Hash Hash Hash

**Two implementations of a hash table (v1 & v2) that is safe to use concurrently**

## Building

To build the program, type
`make`

## Running

To run the program, type
`./hash-table-tester -t [POSITIVE INT] -s [POSITIVE INT]`
where -t represents the number of threads and -s is for the 
number of hash table entries per thread. 
(Default: 4 threads and 25000 entries)

This should print out the times using the
base hash table, v1 table, and v2 table.

Example:

`./hash-table-tester -t 2 -s 50000` 

    Generation: 17,062 usec
    Hash table base: 55,596 usec
    - 0 missing
    Hash table v1: 67,128 usec
    - 0 missing
    Hash table v2: 33,786 usec
    - 0 missing


## Cleaning up

To clean up the program and object files, type
`make clean`


## v1 Implementation

hash_table_v1 is implemented by using a single mutex to 
lock the entire hash table whenever an entry is added to 
the table. This is correct in preventing any data races
since the reading, writing, or updating the table is
blocked whenever another thread is accessing it. However,
this eliminates any benefits of using multiple threads
and actually makes it slower than the base table due to
the overhead of the mutex.

Example output showing v1 is slower than base due to overhead

    Hash table base: 1,071,318 usec
    - 0 missing
    Hash table v1: 1,659,999 usec
    - 0 missing


## v2 Implementation

hash_table_v2 is implemented by having a mutex for each
bucket/linked list in the hash table. Each mutex is locked 
before altering the linked list (and unlocked after) and
thus, this prevents any data races. This implementation
is better than v1 since it allows multiple threads to alter
the hash table at the same time if they are accessing
different linked list in the hash table.


##### Here is an example output testing various thread counts

```./hash-table-tester -t 8 -s 40000
Generation: 53,379 usec
Hash table base: 598,542 usec
- 0 missing
Hash table v1: 757,637 usec
- 0 missing
Hash table v2: 124,634 usec
- 0 missing

./hash-table-tester -t 4 -s 80000
Generation: 52,794 usec
Hash table base: 580,141 usec
- 0 missing
Hash table v1: 895,672 usec
- 0 missing
Hash table v2: 258,009 usec
- 0 missing

./hash-table-tester -t 2 -s 160000
Generation: 53,247 usec
Hash table base: 621,712 usec
- 0 missing
Hash table v1: 859,215 usec
- 0 missing
Hash table v2: 387,859 usec
- 0 missing

./hash-table-tester -t 1 -s 320000
Generation: 53,880 usec
Hash table base: 855,509 usec
- 0 missing
Hash table v1: 1,098,966 usec
- 0 missing
Hash table v2: 1,281,652 usec
- 0 missing
```

**Observations:**

Processing the same number of entries (320000), v2 speeds 
up if we add more threads. Going from 1 to 2 to 4 to 8, the 
time speeds up from 1.2 million to 387,000 to 258,000 to 
124,000.

When the thread count is one, v2 usually performs the 
slowest since it has the most overhead to due having 
multiple mutexes  (ran it with 4096 mutexes). 

v1 typically always runs slower than the base table for any
thread count due to the implementation of locking the entire
hash table. Thus, there are no benefits with multiple threads
so it runs slower than the base table due to the overhead for
the mutex. 

When the thread count is greater than 1, v2 typically runs
(n-3) to (n-1) faster than the base table.


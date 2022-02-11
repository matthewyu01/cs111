# You Spin Me Round Robin

**Program that simulates Round Robin scheduling of processes**

## Building

To build the program, type
`make`

## Running

To run the program, type
`./rr [FILE] [INTEGER]` where the INTEGER >= 1

This should print out the average waiting and response times for the given processes.

Example:

`./rr processes.txt 3` 

    Average waiting time: 7.00
    Average response time: 2.75


## Cleaning up

To clean up the program and object files, type
`make clean`

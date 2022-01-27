# Pipe Up

**Program that implements the shell pipe operator (|)**

## Building

To build the progarm, type
`make`

## Running

To run the program, type
`./pipe CMD1 [CMD2] [CMD3] ...` with at least one command

Example:

`./pipe ls cat` should be equivalent to `ls | cat`

This should list all your files in your current working directory and concatenate them.

## Cleaning up

To clean up the program and object files, type
`make clean`
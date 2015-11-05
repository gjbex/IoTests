# BlockSize
A code snippet to determine a file system's block size.

## What is it?
1. `block_size.c`: source code
1. `Makefile`: make file to build the executable

## Requirements
Any ANSI C compiler will do, e.g., GNU gcc

## Building
Simply run make:
```bash
$ make
```

## Usage
Run the executable, providing a mount point as an argument, e.g.,
```bash
$ ./block_size '/'
```
The block size is expressed in bytes.

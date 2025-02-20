# x86-64-HashTable

## About The Project & Core Functionalities
This project involves an implementation of a hash table in x86-64 Assembly that uses chaining to handle collisions. The implementation involves all the core functionalities that any typical hash table has such as:
- Initialization
- A cutom string hashing algorithm that takes inspiration from the **djb2** hashing algorithm and Java's ```String hashCode()``` function
- Inserting a key-value pair
- Checking if a specific key exists
- Retrieving the value associated with an existing key
- Updating the value of an existing key
- Deleting a key-value pair
- Printing the contents of all slots in the hash table
- Clearing the contents of all slots in the hash table

## Hash Table Structure

If you're interested in taking a look at the structs that are used to define the hash table, you should check out ```src/hash-table.h```. When you scroll down a bit past the constant definitions, you will see two structs declared: ```Node``` and ```Table```. Here are some important things to know about each of them:
1. ```Table```
    - The struct that actually defines the hash table that consists of 4 fields
    - ```long maxWords``` represents the maximum capacity of the hash table in terms of number of key-value pairs it can hold. This field is set by default from the start itself when the hash table is initialized and is used to dictate how many slots will be made available.
    - ```long nWords``` represents the current number of key-value pairs in the hash table
    - ```long nBuckets``` represents the number of slots available in the hash table. The value of this field is based on the ```maxWords``` field in order to ensure that space isn't wasted.
    - ```struct HashTableElement ** array``` represents the actual table into which new key-value pairs will be inserted. Each slot in the table contains a pointer to a chain of nodes
2. ```Node```
    - The struct that defines the contents of each slot in the hash table
    - ```char * word``` represents the key with ```long value``` representing the corresponding value.
    - ```struct HashTableElement * next``` represents a pointer to the next node in the chain. During the insertion process, new key-value pairs, if at all they hash to the same index are inserted at the end of the chain

## Project Structure
Since there are quite a few directories and files in this project, the visualization below provides a means to better understand the purpose of each file on a high level.
```
├── Makefile
├── README.md
├── src
│   ├── Benchmarks: Contains the files associated with benchmarking both the old and optimized x86-64 implementations
│   │   ├── benchmark.h
│   │   ├── hash-table-benchmark-old.c: For benchmarking the old x86-64 Assembly implementation
│   │   └── hash-table-benchmark-opt.c: For benchmarking the optimized x86-64 Assembly implementation
│   ├── New: Contains the files associated with the optimized implementation of the hash table
│   │   ├── hash-table-opt.c: Optimized implementation in C
│   │   └── hash-table-opt.s: Optimized implementation in x86-64 Assembly
│   ├── Old: Contains the files associated with the optimized implementation of the hash table
│   │   ├── hash-table-old.c: Old implementation in C
│   │   └── hash-table-old.s: Old implementation in x86-64 Assembly
│   ├── Utils: Contains files associated with a custom string function library
│   │   ├── str.c: In-line implementations for strlen, strncpy, strncmp, strdup
│   │   └── str.h
│   ├── Words: Contains files that will be used for benchmarking both the old and optimized x86-64 implementations
│   │   ├── 1000w.txt: List of 1000 unique words to be used for benchmarking insertion, lookup and delete
│   │   └── non-existent.txt: List of 500 unique words to be used for benchmarking lookup and delete
│   ├── hash-table.h
│   └── runall.c: Utility script used to run all the tests at once 
└── tests
    ├── New: Contains tests written for the optimized implementations of the hash table
    │   ├── hash-table-asm-test-opt.c: Optimized x86-64 implementation tests
    │   └── hash-table-c-test-opt.c: Optimized C implementation tests
    ├── Old: Contains tests written for the old implementations of the hash table
    │   ├── hash-table-asm-test-old.c: Old x86-64 implementation tests
    │   └── hash-table-c-test-old.c: Old C implementation tests
    └── tests.h
```

## Optimizations Implemented
Beyond the core functionalities implemented for the hash table several optimizations were also implemented. Here are some of the major ones:
1. Modified the bucket sizes used in the ```init``` function to all be a power of 2 instead of prime numbers
2. Made the hashing function run faster by using bitwise operations both in the main loop and at the end when performing remainder division
3. Inlined all the string functions being used such as ```strncmp```, ```strlen```, ```strncpy``` and ```strdup```.
4. Aligned all keys to 8 bytes(i.e, making them all 32 bytes) and modified ```strncmp``` to utilize long operations by comparing keys in chunks instead of using byte-by-byte operations
5. Decreased the number of memory accesses in the ```lookup()``` function by detaching a found node from its current position in the chain and re-attaching it at the head so that if the same key is looked up once again, a chain traversal isn't required.

The primary functions that were benchmarked were insertion, lookup and deletion since those are the key operations associated with a hash table. There were significant performance benefits, some of which are outlined as follows:
- **Insertion time** reduced by **32%**
- **Lookup time** reduced by **27%**
- **Deletion time** reduced by **25%**

## Utilizing The Hash Table

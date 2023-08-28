#include <stdio.h>
#include <stdlib.h>
#include "asm.h"

// Define a memory block structure
typedef struct {
    long data[8];
} mem_block;

// Main function
void main() {
    // Introduce a delay loop
    for (int i = 0; i < 1000; i++) {}

    // Print the start tick for the spy's execution
    printf("\n SPY start at tick : %ld", rdtsc());
    
    mem_block a[2000];
    
    // Flush cache for each data element in the array
    for (int i = 0; i < 2000; i++) {
        flush(&a[i].data[0]);   // Flush cache for a specific data element
        mfence();               // Memory fence for synchronization
        ifence();               // Instruction fence for synchronization
    }
    
    // Print the tick when the spy's execution finishes
    printf("\n Finit au tick : %ld\n", rdtsc());
}


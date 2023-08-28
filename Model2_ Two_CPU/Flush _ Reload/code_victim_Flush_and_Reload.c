#include <stdio.h>
#include <stdlib.h>
#include "asm.h"

// Define a memory block structure
typedef struct {
    long data[8];
} mem_block;

// Main function
void main() {
    mem_block a;
    long start, end;

    // Measure time taken for a memory access operation
    start = rdtsc();
    mem_access(&start); // Initial memory access
    end = rdtsc();
    
    // Display victim's first access result and whether it's a hit or miss
    printf("\n Victim first access : %ld \n", end - start);
    if (end - start > 200) {
        printf("miss\n");
    } else {
        printf("hit \n ");
    }

    // Introduce a delay loop
    for (int i = 0; i < 10000; i++) {}

    // Measure time taken for memory access after a flush
    start = rdtsc();
    mem_access(a.data[0]); // Memory access after flush
    end = rdtsc();
    
    // Display victim's second access result and whether it's a hit or miss
    printf("\n Victim first access after flush: %ld \n", end - start);
    if (end - start > 200) {
        printf("miss\n");
    } else {
        printf("hit \n ");
    }

    // Measure time taken for another memory access after a flush
    start = rdtsc();
    mem_access(a.data[0]); // Second memory access after flush
    end = rdtsc();
    
    // Display victim's third access result and whether it's a hit or miss
    printf("\n Victim second access after flush: %ld \n", end - start);
    if (end - start > 200) {
        printf("miss\n");
    } else {
        printf("hit \n ");
    }
}


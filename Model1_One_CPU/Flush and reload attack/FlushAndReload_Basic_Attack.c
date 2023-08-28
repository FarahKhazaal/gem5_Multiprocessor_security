#include <stdio.h>
#include "asm.h"

// Declare an array to hold data and a file pointer for results
long a[50];
FILE *results_file;

// Function to flush cache addresses
void flush_address () {
    for(int i = 0; i < 50; i++) {
        flush(&a[i]);   // Flush cache for a specific address
        mfence();       // Memory fence for synchronization
        ifence();       // Instruction fence for synchronization
    }
}

// Function to measure access time from the victim's side
void access_time_victim_side() {
    long start, end;
    for (int i = 0; i < 50; i++) {
        mfence();
        ifence();
        start = rdtsc();    // Start measuring cycle count
        mem_access(&a[i]);  // Simulate memory access
        end = rdtsc();      // End measuring cycle count
        mfence();
        ifence();
        fprintf(results_file, "%ld\n", end - start);   // Record time difference
    }
}

// Function to measure access time from the attacker's side
void access_time_attacker_side() {
   fprintf(results_file, "\n\n Attacker Results\n\n");
   long start, end;
   for (int i = 0; i < 50; i++) {
        mfence();
        ifence();
        start = rdtsc();
        mem_access(&a[i]);
        end = rdtsc();
        mfence();
        ifence();
        fprintf(results_file, "%ld\n", end - start);
    }
}

// Function to introduce a delay
void Delay(int counter) {
    for (int i = 0; i < counter; i++) {}
}

// Main function
void main() {
    // Open a results file for writing
    results_file = fopen("access_results.csv", "w");
    
    // Check if file opened successfully
    if (results_file == NULL) {
        perror("Error opening file");
        return;   // Exit if file opening failed
    }

    flush_address();              // Flush cache addresses
    Delay(1000);                  // Introduce a delay
    access_time_victim_side();    // Measure access time from victim's side
    access_time_attacker_side();  // Measure access time from attacker's side
    
    fclose(results_file);         // Close the results file

    printf("Results saved to file: access_results.csv\n");
}


#include <stdio.h>
#include <stdlib.h>

// Includes rdtsc and mem access instructions
#include "asm.h"
// Includes wait function
#include "tri.h"

// Definition of a structure to directly fill 64 byte per block
typedef struct {
    long data[8];
} mem_block;

void main() {

	// Creation of the table
	mem_block a[2500];
	rdtsc();
	// Wait for attacker to fills the cache
	wait(200000);
	
	// Print are used to ensure correct execution of the process
	printf("\n 2) Victim starting reading tick : %ld ", rdtsc()%1000000000000000000);
	for (int i=0;i<2500;i++){
		// Fences to avoid pre-fecthing
		mfence();
		ifence();
		mem_access(&a[i].data[0]);
    	}
    	
    	printf("\n 3) Victim ended reading tick : %ld ", rdtsc()%1000000000000000000);
}


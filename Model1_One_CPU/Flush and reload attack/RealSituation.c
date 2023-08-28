#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "asm.h"

// Define functions for multiplication and addition
long multiplication(long x) {
    return x * 12345; // Modify this key value
}

long addition(long x) {
    return x + 98765; // Modify this key value
}

// Function to flush cache for specified operations
void flush_operation() {
    flush(&addition);
    mfence();
    ifence();
    flush(&multiplication);
    mfence();
    ifence();
}

// Function for second participant to read and detect encryption operations
long second_participant_read() {
    long stolen = 0;
    long encryptedkey = 0;
    long start, end, diff_add, diff_mult;
    
    // Measure access time to 'addition' function
    start = rdtsc();
    mem_access(&addition);
    end = rdtsc();
    diff_add = end - start;
    
    // Measure access time to 'multiplication' function
    start = rdtsc();
    mem_access(&multiplication);
    end = rdtsc();
    diff_mult = end - start;
    
    if (diff_mult < 200) {
        stolen = 7; // multiplication detected
    } 
    
    if (diff_add < diff_mult) {
        stolen = 9; // addition detected
    } 
    
    return stolen;
}

// Function for first participant to access and encrypt data
long first_participant_access(long key) {
    long encryptedkey, encryptedkey1, encryptedkey2, encryptedkey3;

    encryptedkey = multiplication(key); 
    flush(&addition);
    mfence();
    ifence();
    printf("encrypted: %ld \n ", encryptedkey );

    encryptedkey1 = addition(encryptedkey); 
    flush(&multiplication);
    mfence();
    ifence();
    printf("encrypted: %ld \n ", encryptedkey1 );

   
    encryptedkey2 = addition(encryptedkey1); 
    flush(&multiplication);
    mfence();
    ifence();
    printf("encrypted: %ld \n ", encryptedkey2 );
    

    encryptedkey3 = addition(encryptedkey2); 
    flush(&multiplication);
    mfence();
    ifence();
    printf("encrypted: %ld \n ", encryptedkey3 );
    
    return encryptedkey3;
}

// Main function
int main() {
    printf("\n\n Starting execution ... \n\n");
 
    long sensitivedata = 0; 
    printf("Give me your sensitive data to do the encryption algorithm: ");
    scanf("%ld", &sensitivedata);
    
    flush_operation();
    long encrypted_message = first_participant_access(sensitivedata);
    long decrypted_message = encrypted_message;
    long operations_detected = second_participant_read();
    
    // Array to store detected operations
    int detected_operations[100] = {0};

    // Detect and record operations
    for (int i = 0; i < 4; i++) {
        detected_operations[i] = second_participant_read();
        printf("the detected operations : %d \n ", detected_operations[i]);
    }
    
    printf("\n Encrypted message: %ld\n", encrypted_message);
    
    // Decrypt the message based on detected operations
    for (int i = 3; i >= 0; i--) {
        if (detected_operations[i] == 7) {
            decrypted_message -= 98765;
            printf(" %d \t \t %ld \n ", i ,decrypted_message);
        }
        else if  (detected_operations[i] == 9) {
            decrypted_message /= 12345;
            printf(" %d \t \t %ld \n ", i ,decrypted_message);
        }
    }

    printf("\n Decrypted message: %ld\n", decrypted_message);
    
    flush_operation();
    
    return 0; // Indicate successful execution
}


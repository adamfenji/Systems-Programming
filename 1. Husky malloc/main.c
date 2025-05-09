//Adam Fenjiro
//CS3411
//Program 1 - main.c

#include "hmalloc.h"
#include <stdio.h>
#include <string.h>

int main() {

    printf("Test 1: Allocating 100 bytes with hmalloc.\n");
    void *pointer1 = hmalloc(100);
    printf("Allocated at: %p\n", pointer1);
    traverse();
    // Expected Output:
    // Test 1: Allocating 100 bytes with hmalloc.
    // Allocated at: 0x...  (some memory address)

    printf("\nTest 2: Allocating 50 bytes with hcalloc (should be zeroed).\n");
    void *pointer2 = hcalloc(50);
    printf("Allocated at: %p\n", pointer2);
    int zeroed = 1;
    for (int i = 0; i < 50; i++) {
        if (((char *)pointer2)[i] != 0) {
            zeroed = 0;
            break;
        }
    }
    printf("Memory zeroed: %s\n", zeroed ? "Yes" : "No");
    // Expected Output:
    // Test 2: Allocating 50 bytes with hcalloc (should be zeroed).
    // Allocated at: 0x...  (some memory address)
    // Memory zeroed: Yes

    printf("\nTest 3: Freeing pointer1 (100 bytes).\n");
    hfree(pointer1);
    printf("Traversing the free list after freeing pointer1:\n");
    traverse();
    // Expected Output:
    // Test 3: Freeing pointer1 (100 bytes).
    // Traversing the free list after freeing pointer1:
    // Index: 0, Address: 0x..., Length: 100

    printf("\nTest 4: Freeing pointer2 (50 bytes).\n");
    hfree(pointer2);
    printf("Traversing the free list after freeing pointer2:\n");
    traverse();
    // Expected Output:
    // Test 4: Freeing pointer2 (50 bytes).
    // Traversing the free list after freeing pointer2:
    // Index: 0, Address: 0x..., Length: 50
    // Index: 1, Address: 0x..., Length: 100

    printf("\nTest 5: Allocating 80 bytes with hmalloc (should reuse free memory).\n");
    void *pointer3 = hmalloc(80);
    printf("Allocated at: %p\n", pointer3);
    printf("Traversing the free list after allocating 80 bytes:\n");
    traverse();
    // Expected Output:
    // Test 5: Allocating 80 bytes with hmalloc (should reuse free memory).
    // Allocated at: 0x...  (memory address reusing free block)
    // Traversing the free list after allocating 80 bytes:
    // Index: 0, Address: 0x..., Length: 50

    printf("\nTest 6: Allocating 200 bytes with hmalloc (new allocation required).\n");
    void *pointer4 = hmalloc(200);
    printf("Allocated at: %p\n", pointer4);
    traverse();
    // Expected Output:
    // Test 6: Allocating 200 bytes with hmalloc (new allocation required).
    // Allocated at: 0x...  (new memory address)
    // Traversing the free list after allocating 200 bytes:
    // Index: 0, Address: 0x..., Length: 50

    printf("\nTest 7: Freeing pointer3 (80 bytes) and pointer4 (200 bytes).\n");
    hfree(pointer3);
    hfree(pointer4);
    printf("Traversing the free list after freeing pointer3 and pointer4:\n");
    traverse();
    // Expected Output:
    // Test 7: Freeing pointer3 (80 bytes) and pointer4 (200 bytes).
    // Traversing the free list after freeing pointer3 and pointer4:
    // Index: 0, Address: 0x..., Length: 50
    // Index: 1, Address: 0x..., Length: 80
    // Index: 2, Address: 0x..., Length: 200

    printf("\nTest 8: Allocating 0 bytes with hmalloc.\n");
    void *pointer5 = hmalloc(0);
    printf("Allocated at: %p\n", pointer5);
    traverse();
    // Expected Output:
    // Test Case 8: Edge Case - Allocate 0 bytes
    // Expected Output:
    // Test 8: Allocating 0 bytes with hmalloc.
    // Allocated at: (null or no meaningful address)

    printf("\nTest 9: Extra Credit - Testing hrealloc\n");

    // Test 9.1: Allocating 100 bytes
    printf("Allocating 100 bytes with hmalloc.\n");
    void *pointer9_1 = hmalloc(100);
    printf("Allocated at: %p\n", pointer9_1);
    traverse();

    printf("Test 9.2: Reallocating ptr9_1 to 150 bytes with hrealloc (should resize).\n");
    pointer9_1 = hrealloc(pointer9_1, 150);
    printf("Reallocated at: %p\n", pointer9_1);
    traverse();

    printf("Test 9.3: Reallocating ptr9_1 to 50 bytes with hrealloc (should return the same block).\n");
    void *old_pointer9_1 = pointer9_1;
    pointer9_1 = hrealloc(pointer9_1, 50);
    printf("Reallocated at: %p (should be the same as before)\n", pointer9_1);
    if (pointer9_1 == old_pointer9_1) {
        printf("Pointer remains the same, resize not needed.\n");
    } else {
        printf("Pointer changed (unexpected).\n");
    }
    traverse();

    printf("Test 9.4: Reallocating ptr9_1 to 0 bytes (should free the block).\n");
    pointer9_1 = hrealloc(pointer9_1, 0);
    printf("After freeing ptr9_1, pointer is: %p (should be NULL)\n", pointer9_1);
    traverse();

    return 0;
}

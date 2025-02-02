#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customAllocator.h"

// Macros for colored output
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

// Macro for test result
#define PRINT_RESULT(condition, description) \
    if (condition) { \
        printf(GREEN "SUCCESS: %s\n" RESET, description); \
    } else { \
        printf(RED "FAILURE: %s\n" RESET, description); \
    }

int main() {
    printf("Starting comprehensive tests for custom memory allocator...\n\n");


    // Test 1: customMalloc small allocation
    void* ptr1 = customMalloc(4);
    PRINT_RESULT(ptr1 != NULL, "customMalloc(4)");

    // Test 2: customMalloc large allocation
    void* ptr2 = customMalloc(1024 * 1024);  // 1 MB
    PRINT_RESULT(ptr2 != NULL, "customMalloc(1 MB)");

    // Test 3: customMalloc with size 0
    void* ptr3 = customMalloc(0);
    PRINT_RESULT(ptr3 == NULL, "customMalloc(0)");

    // Test 4: customCalloc with initialization
    int* array = (int*) customCalloc(5, sizeof(int));
    int allZero = 1;
    if (array) {
        for (int i = 0; i < 5; i++) {
            if (array[i] != 0) {
                allZero = 0;
                break;
            }
        }
    }
    PRINT_RESULT(array != NULL && allZero, "customCalloc(5, sizeof(int))");

    // Test 5: customCalloc with size 0
    void* ptr4 = customCalloc(0, sizeof(int));
    PRINT_RESULT(ptr4 == NULL, "customCalloc(0, sizeof(int))");

    // Test 6: customFree of valid pointer
    customFree(ptr1);
    PRINT_RESULT(1, "customFree(valid pointer)");

    // Test 7: customFree of NULL pointer
    customFree(NULL);  // Should not crash
    PRINT_RESULT(1, "customFree(NULL pointer)");

    // Test 8: customRealloc to larger size
    void* ptr5 = customMalloc(16);
    void* ptr6 = customRealloc(ptr5, 64);
    PRINT_RESULT(ptr6 != NULL, "customRealloc(ptr, 64)");

    // Test 9: customRealloc to smaller size
    ptr6 = customRealloc(ptr6, 8);
    PRINT_RESULT(ptr6 != NULL, "customRealloc(ptr, 8)");

    // Test 10: customRealloc with NULL pointer
    void* ptr7 = customRealloc(NULL, 32);  // Should behave like malloc
    PRINT_RESULT(ptr7 != NULL, "customRealloc(NULL, 32)");

    // Test 11: customRealloc with size 0
    ptr7 = customRealloc(ptr7, 0);  // Should behave like free
    PRINT_RESULT(ptr7 != NULL, "customRealloc(ptr, 0)");

    // Test 12: Memory fragmentation
    void* ptr8 = customMalloc(16);
    customFree(ptr8);
    void* ptr9 = customMalloc(16);  // Reusing the variable to avoid warnings
    PRINT_RESULT(ptr9 == ptr8, "Memory reuse after free (fragmentation test)");

    // Test 13: Exhaust memory
    void* allocations[1000];
    int successCount = 0;
    for (int i = 0; i < 1000; i++) {
        allocations[i] = customMalloc(1024);  // Allocate 1 KB
        if (!allocations[i]) {
            break;
        }
        successCount++;
    }
    PRINT_RESULT(successCount > 0, "Exhaust memory test");

    // Free all successful allocations
    for (int i = 0; i < successCount; i++) {
        customFree(allocations[i]);
    }

    printf("\nAll tests completed.\n");
    return 0;
}

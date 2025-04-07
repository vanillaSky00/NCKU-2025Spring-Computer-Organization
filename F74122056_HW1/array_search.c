#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;

    asm volatile(
        // Your code
        /**
         * 1. place result, target, arra_size into three registers
         * 2. check arr size if 0 then branch out to leave result still -1
         * 3. set a variable i for counting loops
         * 4. bne(compared to target register) to the loop if not find the a[i]
         * 5. if equal then assign i to result
         * 
         */
        
        "add t0, %[cnt], x0          \n\t" //t0=arr_size
        "add t1, %[res], x0          \n\t" //t1=result
        "add t2, %[tar], x0          \n\t" //t2=target
        
        "beq t0, x0, done_search     \n\t" //if arr_size=0 skip search

        "addi t3, x0, 0              \n\t"

        "search_loop:                \n\t"
        "lw t4, 0(%[A])              \n\t"   
        "beq t4, t2, found           \n\t"
        "addi %[A], %[A], 4          \n\t"
        "addi t3, t3, 1              \n\t"
        "ble t3, t0, search_loop     \n\t"
        
        "j done_search               \n\t"

        "found:                      \n\t"
        "add t1, t3, 0               \n\t"

        "done_search:                \n\t"
        "add %[res], t1, x0          \n\t"//write it back
        : [res] "+r"(result)
        : [A] "r"(p_a), [cnt] "r"(arr_size), [tar] "r"(target)
        : "t0", "t1", "t2", "t3", "t4"
        );

    return result;
}

// Main function to test the implementation
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int arr_size;
    fscanf(input, "%d", &arr_size);
    int arr[arr_size];

    // Read integers from input file into the array
    for (int i = 0; i < arr_size; i++) {
        int data;
        fscanf(input, "%d", &data);
        arr[i] = data;
    }
    int target;
    fscanf(input, "%d", &target);
    fclose(input);

    int *p_a = &arr[0];

    int index = arraySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}

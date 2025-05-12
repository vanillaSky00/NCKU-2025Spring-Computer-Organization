#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;

    asm volatile(
        // Your code
	/**
         * 1. check arr size if 0 then branch out to leave result still -1
         * 2. set a variable i for counting loops
         * 3. compared to target register
         * 4. if equal then assign i to result
         * 
         */
        "beq %[cnt], x0, done_search \n\t"
        
        "addi t3, x0, 0              \n\t"
	"add t5, %[A], x0            \n\t"

        "search_loop:                \n\t"
        "lw t4, 0(t5)                \n\t"   
        "beq t4, %[tar], found       \n\t"
        "addi t5, t5, 4              \n\t"
        "addi t3, t3, 1              \n\t"
        "blt t3, %[cnt], search_loop \n\t"
        "j not_found                 \n\t"

        "found:                      \n\t"
	"add %[res], t3, x0          \n\t"
        "j done_search               \n\t"
	
	"not_found:                  \n\t"
	"addi %[res], x0, -1         \n\t"

        "done_search:                \n\t"
        
	: [res] "+r" (result)
        : [A] "r" (p_a), [cnt] "r" (arr_size), [tar] "r" (target)
        : "t3", "t4", "t5"
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

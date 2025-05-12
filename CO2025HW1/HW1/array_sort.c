#include <stdio.h>

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
    fclose(input);

    int *p_a = &arr[0];

    // array a bubble sort
    /* Original C code segment
    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i -1; j++) {
            if (*(p_a + j) > *(p_a + j + 1)) {
                int tmp = *(p_a + j);
                *(p_a + j) = *(p_a + j + 1);
                *(p_a + j + 1) = tmp;
            }
        }
    }
    */

    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            asm volatile(
                // Your code
		"add t0, %[j], x0   \n\t"//t0=j
                "slli t0, t0, 2     \n\t"//i element 4 byte
                "add t2, %[A], t0   \n\t"//t2 store the address of a[j]

                "lw t5, 0(t2)       \n\t"//t5=a[j]

                "addi t0, t0, 4     \n\t"//
                "add t3, %[A], t0   \n\t"//t3 store the address of a[j+1]
                "lw t6, 0(t3)       \n\t"//t6=a[j+1]

                "ble t5, t6, done   \n\t"//compare, if t5<t6 no swap, order is correct

                "sw t5, 0(t3)       \n\t"
                "sw t6, 0(t2)       \n\t"

                "done:              \n\t"

                : [A] "+r"(p_a)
                : [j] "r" (j)
                : "t0", "t2", "t3", "t5", "t6"
                );
        }
    }
    p_a = &arr[0];
    for (int i = 0; i < arr_size; i++)
        printf("%d ", *p_a++);
    printf("\n");
    return 0;
}

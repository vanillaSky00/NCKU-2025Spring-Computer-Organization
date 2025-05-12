#include <stdint.h>
#include <stdio.h>

int main()
{
    int32_t a[10] = {0}, b[10] = {0}, c[10] = {0};
    int32_t i, arr_size = 10;
    FILE *input = fopen("../testcases/input/2.txt", "r");
    for (i = 0; i < arr_size; i++)
        fscanf(input, "%d", &a[i]);
    for (i = 0; i < arr_size; i++)
        fscanf(input, "%d", &b[i]);
    fclose(input);
    int32_t *p_a = a;
    int32_t *p_b = b;
    int32_t *p_c = c;
    /* Original C code segment
    for (int32_t i = arr_size; i != 0; i--) 
        *p_c++ = *p_a++ - *p_b++;
    */
    asm volatile("add t0, %[cnt], x0 \n\t"
		 "loop:              \n\t"    
		 "lw t1, 0(%[A])     \n\t"
		 "lw t2, 0(%[B])     \n\t"
		 "sub t1, t1, t2     \n\t"
		 "sw t1, 0(%[C])     \n\t"
		 "addi t0, t0,-1     \n\t"
		 "addi %[A], %[A], 4 \n\t"
		 "addi %[B], %[B], 4 \n\t"
		 "addi %[C], %[C], 4 \n\t"
		 "bnez t0, loop      \n\t"
		 :[A] "+r"(p_a), [B] "+r"(p_b), [C] "+r"(p_c)
		 :[cnt] "r"(arr_size)
		 :"t0", "t1", "t2"
		);

    p_c = c;
    for (int32_t i = 0; i < arr_size; i++)
        printf("%d ", *p_c++);
    printf("\n");
    return 0;
}

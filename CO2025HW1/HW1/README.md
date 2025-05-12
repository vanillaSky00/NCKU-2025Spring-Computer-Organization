### HW1 - Bubble Sort in Array/ Merge Sort in Linked List
The goal is to integrate assembly instructions directly within C code to handle the sorting operations at a low level, providing more control over the underlying hardware.

### Bubble Sort
I initially started by implementing the loop using inline assembly to perform comparisons and swaps.

I used a "done" flag section that would be triggered once the array is sorted, allowing the loop to exit early.

To ensure better readability, I separated the loop section and done flag section, using an unconditional jump to the appropriate location based on conditions. This made the control flow cleaner and more modular.

### Merge Sort


### Code Refactorization 

Input values should only be stored in registers when modification is needed. If no modification (e.g., comparison) is required, there's no need to store the values in registers.
```
//bubble_sort
asm volatile(
    // Your code
    "add t0, %[j], x0  \n\t"//t0=j
    "slli t0, t0, 2     \n\t"//i element 4 byte
    "add t2, %[A], t0  \n\t"//t2 store the address of a[j]
    
    "lw t5, 0(t2)      \n\t"//t5=a[j]

    "addi t0, t0, 4    \n\t"//
    "add t3, %[A], t0  \n\t"//t3 store the address of a[j+1]
    "lw t6, 0(t3)      \n\t"//t6=a[j+1]

    "bgt t5, t6, swap  \n\t"//compare

    "j done            \n\t"

    "swap:             \n\t"
    "sw t5, 0(t3)      \n\t"
    "sw t6, 0(t2)      \n\t"

    "done:             \n\t"

    : [A] "+r" (p_a)
    : [j] "r" (j)
    : "t0", "t2", "t3", "t5", "t6"
    );  
```
refactorize to
```
asm volatile(
    // Your code
    "add t0, %[j], x0  \n\t"
    "slli t0, t0, 2    \n\t"
    "add t2, %[A], t0  \n\t"

    "lw t5, 0(t2)      \n\t"

    "addi t0, t0, 4    \n\t"
    "add t3, %[A], t0  \n\t"
    "lw t6, 0(t3)      \n\t"

    "ble t5, t6, done  \n\t"//compare, if t5<t6 no swap, order is correct

    "sw t5, 0(t3)      \n\t"
    "sw t6, 0(t2)      \n\t"

    "done:             \n\t"

    : [A] "+r"(p_a)
    : [j] "r" (j)
    : "t0", "t2", "t3", "t5", "t6"
    );
}
```


```
//array search
asm volatile(
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
    : [res] "+r" (result)
    : [A] "r" (p_a), [cnt] "r" (arr_size), [tar] "r" (target)
    : "t0", "t1", "t2", "t3", "t4"
    );
```
refactorize to:

```
asm volatile(
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
```

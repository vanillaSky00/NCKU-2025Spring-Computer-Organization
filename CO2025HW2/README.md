## Assignment Summary
This assignment focuses on **performance modeling and instruction-level optimization** on a RISC-V processor using inline assembly. 
- F/D extensions for single/double-precision floating point

- M extension for multiplication/division

- V extension for vector processing
## Objectives:
```
- Implement mathematical operations (FFT, array multiplication, etc.)

- Collect instruction counts using counters like ```add_cnt```, ```fmul_cnt```, etc.

- Compute total cycle count and CPU time using predefined CPI values

- Characterize your code as CPU-bound or Memory-bound using the provided performance ratio formula
```
## Implementation Tasks:

```
1. FFT:
   1-1: Implement Cooley-Tukey radix-2 DIF FFT using inline assembly
   1-2: Complete core functions: complex_add, complex_mul, bit_reverse, and log2

2. Array Multiplication:
   2-1: Implement float array multiplication (baseline, no V extension)
   2-2: Rewrite using V extension for performance comparison

3. Float/Double Reduction Multiplication:
   3-1: Multiply all float elements of two arrays (float version)
   3-2: Repeat using double-precision, then compare slowdown and relative error

```

## Performance Analysis
```
total_cycle_count = Σ(counter × corresponding_CPI)
cpu_time = total_cycle_count × cycle_time
```
To determine whether your program is **CPU-bound** or **Memory-bound**, use the following ratio:

$`
\text{Ratio} = \frac{\text{CPU cycles (non-load/store)}}{\text{Total cycles}}
`$

- If the ratio \( > 0.5 \), the program is **CPU-bound**.
- If the ratio \( \leq 0.5 \), the program is **Memory-bound**.
  
## V Extension
use RISC-V vector instructions (`vle32.v`, `vfmul.vv`, `vfadd.vf`, etc.) for element-wise operations on arrays using 32-bit float vectors.
```
//use V extension
"loop:"                                             //e32 tell the processor use 
    "vsetvli t0, %[arr_size], e32             \n\t" //set vector len to fit 32-bit
    "sub %[arr_size], %[arr_size], t0         \n\t" //decrease len of arr_size
    "vle32.v v0, 0(%[h])                      \n\t" //Load h[0..VL-1] → v0
    "vle32.v v1, 0(%[x])                      \n\t" //Load x[0..VL-1] → v1
    "vfmul.vv v2, v0, v1                      \n\t" //v2[i] = v0[i] * v1[i]
    "vfadd.vf v2, v2, %[id]                   \n\t" //v2[i] += id
    "vse32.v v2, (%[y])                       \n\t" //Store v2 → y[0..VL-1]

    "slli t0, t0, 2                           \n\t"
    "add %[h], %[h], t0                       \n\t"
    "add %[x], %[x], t0                       \n\t"
    "add %[y], %[y], t0                       \n\t"
    "bnez %[arr_size], loop                   \n\t"
```
## Note
- [Early Clobber in Inline ASM](#early-clobber-in-inline-asm)
- [Approximation to Pi](#approximation-to--pi)
- [Bit Reverse](#bit-reverse)
- [Single vs Double Precision](#single-vs-double-precision)
- [Common Mistakes in `rem`](#common-mistakes-in-rem)


### Early Clobber in Inline ASM 
You must use `=&f` for outputs modified before all inputs are read to prevent register hazards.
```
Wrong: [C_Re] "=f"
Correct: [C_Re] "=&f"
```
The below shows the problem
```
Complex complex_mul(Complex a, Complex b)
{
    Complex result;
    asm volatile(
        "addi %[fmul_cnt], %[fmul_cnt], 1\n\t"
        "fmul.s f1, %[A_Re], %[B_Re]       \n\t"
        "fmul.s f2, %[A_Im], %[B_Im]       \n\t"
        "fsub.s %[C_Re], f1, f2            \n\t"

        "fmul.s f3, %[A_Re], %[B_Im]       \n\t"
        "fmul.s f4, %[A_Im], %[B_Re]       \n\t"
        "fadd.s %[C_Im], f3, f4            \n\t"

        : [C_Re] "=f"(result.Re), [C_Im] "=f"(result.Im), [fmul_cnt] "+r"(fmul_cnt), [fsub_cnt] "+r"(fsub_cnt), [fadd_cnt] "+r"(fadd_cnt)
        : [C_Re] "=f"(result.Re), [C_Im] "=f"(result.Im), [fmul_cnt] "+r"(fmul_cnt), [fsub_cnt] "+r"(fsub_cnt), [fadd_cnt] "+r"(fadd_cnt)
        : [A_Re] "f"(a.Re), [B_Re] "f"(b.Re),
        [A_Im] "f"(a.Im), [B_Im] "f"(b.Im)
        : "f1", "f2", "f3", "f4"
    );
    return result;
}
```

### Approximation to Pi
Approximate π using the Leibniz series:

$`
\pi \approx 4 \left(1 - \frac{1}{3} + \frac{1}{5} - \frac{1}{7} + \frac{1}{9} - \cdots \right)
\pi \approx 4 \sum_{i=0}^{\infty} \frac{(-1)^i}{2i + 1}
`$



### Bit Reverse
Reverse the bits of a number using shift and mask instructions.
You must know how many bits to reverse (m), e.g., for m = 10:
```
1 (0000000001) → 512 (1000000000)

2 (0000000010) → 256 (0100000000)
```
A common mistake for bit reverse:
```
//wrong
uint32_t bit_reverse(uint32_t b, uint32_t m)
{   
    asm volatile (
        "add t2, %[b], x0                      \n\t"
        "addi t0, x0, 0                        \n\t"

        "reverse_loop:                         \n\t"
        "slli t0, t0, 1                        \n\t"
        "andi t1, t2, 1                        \n\t"
        "or t0, t0, t1                         \n\t"
        "srli t2, t2, 1                        \n\t"
        "bnez t2, reverse_loop                 \n\t"
        "add %[b], t0, x0                      \n\t"
        : [b] "+r"(b), [lw_cnt] "+r"(lw_cnt), [add_cnt] "+r"(add_cnt), [sub_cnt] "+r"(sub_cnt), [sw_cnt] "+r"(sw_cnt), [others_cnt] "+r"(others_cnt)
        : [m] "r"(m)
        : "t0", "t1", "t2"
    );
    
    return b;
}
```

### Single vs Double Precision
```
//single float array
"loop:                                         \n\t"
    "flw f0, 0(%[h])                           \n\t"
    "flw f1, 0(%[x])                           \n\t"
    "fmul.s f0, f0, f1                         \n\t"//result *= h_i *x_i
    //flw f1, %[result]                        \n\t wrong cause result is +f is a float register
    "fmul.s %[result], %[result], f0           \n\t"
    //fsw f0, %[result]                         \n\t wrong smae mistake above

    "addi %[h], %[h], 4                        \n\t"
    "addi %[x], %[x], 4                        \n\t"
    "addi %[arr_size], %[arr_size], -1         \n\t"

    "bnez %[arr_size], loop                    \n\t"
```
```
//double float array
"loop:                                         \n\t"
    "fld f0, 0(%[h])                           \n\t"//fld load 64bit float = double
    "fld f1, 0(%[x])                           \n\t"
    "fmul.d f0, f0, f1                         \n\t"//result *= h_i *x_i

    "fmul.d %[result], %[result], f0           \n\t"

    "addi %[h], %[h], 8                        \n\t"//not + 4
    "addi %[x], %[x], 8                        \n\t"
    "addi %[arr_size], %[arr_size], -1         \n\t"

    "bnez %[arr_size], loop                    \n\t"
```

### Common Mistakes in `rem`

exercise1.c:83: Error: illegal operands `rem t3, t0, 2`,  
```
1. cannot use rem on float registers
2. rem's arguments can only be two register
```
use and to extract last bit to check `andi t3, t0, 1`

```
//wrong
"add  t1, x0, x0                    \n\t"//i=0
"faddi.s  t4, x0, 1                 \n\t"//t4=1
"addi %[pi], %[pi], 1               \n\t"//result = 1

"loop:                              \n\t"
    "fmuli.s t2, t1, 2              \n\t"//t2= 2i
    "faddi.s t2, t2, 3              \n\t"//t2=2i+3
    "fdivi.s t2, t4, t2             \n\t"//t2=1/t2
    "rem t3, t1, 2                  \n\t"//t3=i%2

    "bnez t3, add_next              \n\t"
    "sub %[pi], %[pi], t2           \n\t"//1-1/3
    "j end                          \n\t"
    
    "add_next:"
        "add %[pi], %[pi], t2       \n\t"//1-1/3+1/5
    

    "end: "
        "addi t1, t1, -1            \n\t"
        "ble t1, 1000, loop         \n\t"    
```

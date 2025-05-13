## Assignment Goal


## V extension
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

    "addi %[add_cnt], %[add_cnt], 3           \n\t"
    "addi %[sub_cnt], %[sub_cnt], 1           \n\t"
    "addi %[fmul_cnt], %[fmul_cnt], 1         \n\t"//vfmul
    "addi %[fadd_cnt], %[fadd_cnt], 1         \n\t"//vfadd
    "addi %[lw_cnt], %[lw_cnt], 2             \n\t"//vle32 
    "addi %[sw_cnt], %[sw_cnt], 1             \n\t"//vfse32
    "addi %[others_cnt], %[others_cnt], 3     \n\t"//vsetvli,bnez,slli

    "bnez %[arr_size], loop                   \n\t"
```

## early clobber
SHOULD BE [C_Re] "=&f" NOT [C_Re] "=f"
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

        : [C_Re] "=&f"(result.Re), [C_Im] "=f"(result.Im), [fmul_cnt] "+r"(fmul_cnt), [fsub_cnt] "+r"(fsub_cnt), [fadd_cnt] "+r"(fadd_cnt)
        : [C_Re] "=f"(result.Re), [C_Im] "=f"(result.Im), [fmul_cnt] "+r"(fmul_cnt), [fsub_cnt] "+r"(fsub_cnt), [fadd_cnt] "+r"(fadd_cnt)
        : [A_Re] "f"(a.Re), [B_Re] "f"(b.Re),
        [A_Im] "f"(a.Im), [B_Im] "f"(b.Im)
        : "f1", "f2", "f3", "f4"
    );
    //printf("mul operation\n");
    //printf("A_Re:%f,A_Im:%f\n",a.Re,a.Im);
    //printf("B_Re:%f,B_Im:%f\n",b.Re,b.Im);
    //printf("C_Re:%f,C_Im:%f\n",result.Re,result.Im);
    return result;
}
```

## Approximation to PI
Using Lebitnize series
```
```


## Bit reverse
A common mistake for bit reversing:
```
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
We have to know the bit len for a given number. Why?:
```
Bit len (m) = 2
1 -> 2 (01->10)
2 _> 1 (10->01)
```
```
Bit len (m) = 10
1 -> 512 (0000000001 -> 1000000000)
2 -> 256 (00000000010 -> 0100000000)
```
```
//we have to know how many bit to reversed
"add t2, %[m], x0                               \n\t"
"addi t0, x0, 0                                 \n\t"
"addi %[add_cnt], %[add_cnt], 2                 \n\t"
"reverse_loop:                                  \n\t"
    "slli t0, t0, 1                             \n\t"
    "andi t1, %[b], 1                           \n\t"
    "or t0, t0, t1                              \n\t"
    "srli %[b], %[b], 1                         \n\t"
    "addi t2, t2, -1                            \n\t"
    "addi %[add_cnt], %[add_cnt], 1             \n\t"
    "addi %[others_cnt], %[others_cnt], 5       \n\t"
    "bnez t2, reverse_loop                      \n\t"
"add %[b], t0, x0                               \n\t"
"addi %[add_cnt], %[add_cnt], 1                 \n\t"

```
## single/doulbe use different sytax and differen size
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

## common mistake
exercise1.c:83: Error: illegal operands `rem t3,t0,2'   
use and to extract last bit to check "andi t3, t0, 1                           \n\t"
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

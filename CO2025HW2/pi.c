"addi %[lw_cnt], %[lw_cnt], 1\n\t"
"li t0, 1000                                  \n\t"
"addi %[lw_cnt], %[lw_cnt], 1\n\t"
"li t1, 1                                     \n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
"fcvt.s.w f1, t1                              \n\t"

"pi_loop:                                     \n\t"
"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t0, t0, -1                          \n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "slli t2, t0, 1                           \n\t"
"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t2, t2, 1                           \n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "fcvt.s.w f2, t2                          \n\t"
"addi %[fdiv_cnt], %[fdiv_cnt], 1\n\t"
    "fdiv.s f3, f1, f2                        \n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "andi t3, t0, 1                           \n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "beqz t3, even                            \n\t"
"addi %[fsub_cnt], %[fsub_cnt], 1\n\t"
    "fsub.s %[pi], %[pi], f3                  \n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "j end                                    \n\t"
    
    "even:                                    \n\t" 
"addi %[fadd_cnt], %[fadd_cnt], 1\n\t"
    	"fadd.s %[pi], %[pi], f3              \n\t"
    	
    "end: "

"addi %[others_cnt], %[others_cnt], 1\n\t"
        "bnez t0, pi_loop                     \n\t"

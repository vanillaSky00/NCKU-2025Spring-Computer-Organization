"addi %[add_cnt], %[add_cnt], 1              \n\t"
"addi %[others_cnt], %[others_cnt], 1        \n\t"
"addi t0, x0, 1                              \n\t"
"bgt %[N], t0, log_loop                      \n\t"
"j log_end                                   \n\t"

"log_loop:                                   \n\t"
    "srli %[N], %[N], 1                      \n\t"
    "addi %[log], %[log], 1                  \n\t"
    "addi %[add_cnt], %[add_cnt], 1          \n\t"
    "addi %[others_cnt], %[others_cnt], 2    \n\t"
    "bgt %[N], t0, log_loop                  \n\t"

"log_end:                                    \n\t"

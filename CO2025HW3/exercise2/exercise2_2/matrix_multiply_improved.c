#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef UNROLL
#   define UNROLL 32        //8 fits 32-byte block cache config but 32 tested good in overall
#endif

void matrix_multiply(int * A, int * B, int * Output, int i, int k, int j)
{
    //turn B to row major, now we have row*row
    int* Bt = (int *)malloc((size_t)k * j * sizeof(int));
    if(!Bt){           
        perror("Bt malloc");
        return;
    }

    for(int row = 0; row < k; row++){
        for(int col = 0; col < j; col++){
            Bt[(size_t)col * k + row] = B[(size_t)row * j + col];
	}
    }

    //matrix multiply with locality, const:read only, restrict:no alias 
    for(int row = 0; row < i; row++){
        const int* restrict a_row = A + (size_t)row * k;

        for(int col = 0; col < j; col++){//col is row for B
            const int* restrict b_row = Bt + (size_t)col * k;

            int sum = 0;
            int z   = 0;


//manual unroll, improved for around 0.4, avoid branch overhead
#if UNROLL >= 32
            for(; z + 31 < k; z += 32){
                sum += a_row[z     ] * b_row[z     ];
                sum += a_row[z +  1] * b_row[z +  1];
                sum += a_row[z +  2] * b_row[z +  2];
                sum += a_row[z +  3] * b_row[z +  3];
                sum += a_row[z +  4] * b_row[z +  4];
                sum += a_row[z +  5] * b_row[z +  5];
                sum += a_row[z +  6] * b_row[z +  6];
                sum += a_row[z +  7] * b_row[z +  7];
                sum += a_row[z +  8] * b_row[z +  8];
                sum += a_row[z +  9] * b_row[z +  9];
                sum += a_row[z + 10] * b_row[z + 10];
                sum += a_row[z + 11] * b_row[z + 11];
                sum += a_row[z + 12] * b_row[z + 12];
                sum += a_row[z + 13] * b_row[z + 13];
                sum += a_row[z + 14] * b_row[z + 14];
                sum += a_row[z + 15] * b_row[z + 15];
                sum += a_row[z + 16] * b_row[z + 16];
                sum += a_row[z + 17] * b_row[z + 17];
                sum += a_row[z + 18] * b_row[z + 18];
                sum += a_row[z + 19] * b_row[z + 19];
                sum += a_row[z + 20] * b_row[z + 20];
                sum += a_row[z + 21] * b_row[z + 21];
                sum += a_row[z + 22] * b_row[z + 22];
                sum += a_row[z + 23] * b_row[z + 23];
                sum += a_row[z + 24] * b_row[z + 24];
                sum += a_row[z + 25] * b_row[z + 25];
                sum += a_row[z + 26] * b_row[z + 26];
                sum += a_row[z + 27] * b_row[z + 27];
                sum += a_row[z + 28] * b_row[z + 28];
                sum += a_row[z + 29] * b_row[z + 29];
                sum += a_row[z + 30] * b_row[z + 30];
                sum += a_row[z + 31] * b_row[z + 31];
            }
#elif UNROLL >= 16
            for(; z + 15 < k; z += 16){
                sum += a_row[z     ] * b_row[z     ];
                sum += a_row[z +  1] * b_row[z +  1];
                sum += a_row[z +  2] * b_row[z +  2];
                sum += a_row[z +  3] * b_row[z +  3];
                sum += a_row[z +  4] * b_row[z +  4];
                sum += a_row[z +  5] * b_row[z +  5];
                sum += a_row[z +  6] * b_row[z +  6];
                sum += a_row[z +  7] * b_row[z +  7];
                sum += a_row[z +  8] * b_row[z +  8];
                sum += a_row[z +  9] * b_row[z +  9];
                sum += a_row[z + 10] * b_row[z + 10];
                sum += a_row[z + 11] * b_row[z + 11];
                sum += a_row[z + 12] * b_row[z + 12];
                sum += a_row[z + 13] * b_row[z + 13];
                sum += a_row[z + 14] * b_row[z + 14];
                sum += a_row[z + 15] * b_row[z + 15];
            }
#elif UNROLL >= 8
            for(; z + 7 < k; z += 8){
                sum += a_row[z     ] * b_row[z     ];
                sum += a_row[z +  1] * b_row[z +  1];
                sum += a_row[z +  2] * b_row[z +  2];
                sum += a_row[z +  3] * b_row[z +  3];
                sum += a_row[z +  4] * b_row[z +  4];
                sum += a_row[z +  5] * b_row[z +  5];
                sum += a_row[z +  6] * b_row[z +  6];
                sum += a_row[z +  7] * b_row[z +  7];
            }
#elif UNROLL >= 4
            for(; z + 3 < k; z += 4){
                sum += a_row[z    ] * b_row[z    ];
                sum += a_row[z + 1] * b_row[z + 1];
                sum += a_row[z + 2] * b_row[z + 2];
                sum += a_row[z + 3] * b_row[z + 3];
            }
#endif
            //for any leftover elements (k % UNROLL)
            for(; z < k; z++){
                sum += a_row[z] * b_row[z];
	    }
            Output[(size_t)row * j + col] = sum;
        }
    }

    free(Bt);
}


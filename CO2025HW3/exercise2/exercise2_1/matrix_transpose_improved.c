#ifndef BLOCK_SIZE
    #define BLOCK_SIZE 8 //32-byte cacheline = 32/4 = 8 int
#endif


void matrix_transpose(int n, int *dst, int *src) {
    // Inplement your code here
    const int B = BLOCK_SIZE;

    for(int i = 0; i < n; i+=B){
        for(int j = 0; j < n; j+=B){
	    //check if the index is still within bound
	    int i_max = (i + B < n) ? i + B : n;
	    int j_max = (j + B < n) ? j + B : n ;

	    //inside one B×B tile: read a row, write a row → both contiguous
            for (int y = i; y < i_max; y++) {
                const int *row_src  = src + y * n;   //current row in src
                int       *row_dst  = dst + y;       //first element in dst column 

                for (int x = j; x < j_max; x++) {
		    //transpose logic Dst00 <- Src00, Dst0n <- Src01
		    row_dst[x * n] = row_src[x];
                }
            }    
	}
    }
}

## Assigment3 Overall
- How to use FIFO by editing `cachesim.cc`<br> <br/>
- Why and how the provided `matrix_transpose_improved.c` and `matrix_multiply_improved.c` achieve major speed‑ups. Here we introduce three classic loop‑level cache‑optimization techniques—`loop interchange`, `loop fusion`, and `loop tiling—with` easy‑to‑follow C code snippets.
<br> <br/>

## Cache Rule Setting
| Function             | Role in a miss/hit sequence | 
| -------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| `access()`           | Top-level entry: parse address, call `check_tag`, update counters, invoke `victimize` on a miss, perform write-back if needed.   |
| `check_tag()`        | Extract *set index* (`idx = (addr >> idx_shift) & (sets-1)`), linearly compare tags in that set.                                 |
| `victimize()`        | **Original**: uses `lfsr.next() % ways` to pick a random way. <br> <br/> **FIFO** version: pop the head of a per-set queue and push the new tag to its tail. |
| `clean_invalidate()` | Handles explicit cache cleans / invalidations; still uses `victimize` logic.                                                     |
| `print_stats()`      | Aggregates hits, misses, write-backs for evaluation.                                                                             |

<br> <br/>
## Matrix Optimization
### Why caches love contiguous access

Modern CPUs fetch data from main memory into the L1 cache in cache‑lines (typically 64 B). If successive loads/stores hit the same cache‑line, the operation costs ~4 cycles; if they come from different lines it costs hundreds of cycles. All three techniques below rearrange loops so that the next datum you need is already in L1.


### Loop Interchange – improve spatial locality
Change the nesting order so the loop variable that corresponds to contiguous memory advances fastest.
Before (transposing an matrix): column walk causes cache misses
```
for (int y = 0; y < n; ++y)        // outer loop walks DOWN the column
    for (int x = 0; x < n; ++x)    // inner loop walks ACROSS the row
        dst[y + x * n] = src[x + y * n];
```
After: row walk is innermost → stride = 1
```
for (int x = 0; x < n; ++x)        // outer loop now the row index
    for (int y = 0; y < n; ++y)    // inner loop strides through src row
        dst[y + x * n] = src[x + y * n];
```

### Loop Fusion – improve temporal locality
If two loops touch the same array, combine them so each element is brought into cache only once.

Before:
```
for (int i = 0; i < N; ++i)              // pass 1: compute sum[]
    sum[i] = a[i] + b[i];

for (int i = 0; i < N; ++i)              // pass 2: compute avg[] using sum[]
    avg[i] = sum[i] / 2;
```
After (fused):
```
for (int i = 0; i < N; ++i) {
    int s   = a[i] + b[i];   // element loaded once
    sum[i]  = s;
    avg[i]  = s / 2;
}
```

### Loop Tiling (Blocking) – optimise both localities
Before:
```
void matrix_transpose(int n, int *dst, int *src) {
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            dst[y + x * n] = src[x + y * n];
        }
    }
}
```
After (tiling):
```
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
```




### InstaLL simulator
```
cd ~riscv/
sudo apt install device-tree-compiler libboost-regex-dev ;ibboost-all-dev
git clone https://github.com/riscv/riscv-isa-sim.git
mkdir build
cd build
../configure --prefix=$RISCV
make
sudo make install

under /home/CompOrg/riscv/riscv-isa-sim
we have riscv directory
cp riscv/cachesim.h riscv/cachesim.h.orig
cp riscv/cachesim.cc riscv/cachesim.cc.orig
cp /path/to/your/new/cachesim.h  riscv/
cp /path/to/your/new/cachesim.cc riscv/
```
<br> <br/>
## Key insights

| Concept                  | Explanation |
|--------------------------|-------------|
| **Why FIFO > LFSR**      | **LFSR** (Linear Feedback Shift Register) performs pseudo-random replacement and ignores temporal locality — recently-used lines may be evicted just like cold ones.<br><br>**FIFO** ensures that a line stays in the cache for an entire rotation of the set, allowing for short-term temporal reuse. This typically results in a lower miss rate, as demonstrated in the assignment's logs. |
| **Write-back timing**    | A write-back occurs **only when a dirty line is evicted** (i.e., chosen as a victim).<br><br>The data is written back to the **next memory level** — either an L2/L3 cache or main memory (DRAM), depending on cache hierarchy. |
| **Capacity vs Conflict Miss** | **Capacity miss**: Happens when the total working set size exceeds the overall cache capacity — even with full associativity.<br><br>**Conflict miss**: Occurs when multiple addresses map to the same cache set, and the associativity (i.e., number of ways) is too limited to hold all of them, causing evictions even though cache has unused space elsewhere. |

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

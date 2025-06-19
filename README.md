# Computer Organization 2025 — Programming Assignments  
RISC-V ▸ Inline Assembly ▸ Performance Modelling ▸ Cache Optimisation

> This mono-repo gathers my solutions to the three graded programming
> assignments for **EECS / CS, NCKU, Spring 2025**.  
> Each homework is self-contained in its own folder and can be built or
> simulated independently.

---

## Repository Layout
### CO2025HW0: Environment Preparation


### CO2025HW1: Inline Assembly Practice
Implements basic algorithms (`bubble sort`, `linear search`, `merge sort` on `linked lists`) directly using RISC-V inline assembly within C. Focuses on:
- Understanding calling conventions and data alignment
- Practicing `asm volatile` syntax for register-level control
- Gaining hands-on experience with RV64 `I` instruction set


### CO2025HW2: Performance Modelling & Vector Extension
Analyzes the performance of scalar vs. vectorized operations (e.g., FFT, matrix-vector multiply) using:
- RISC-V `F` (floating-point) and `V` (vector) extensions
- CPU vs. memory bound classification
- Instruction-level performance modelling using CPI and cycle counters


### CO2025HW3: Cache Design + Cache-Aware Software Optimization
Divided into two parts:
1. Modify a cache simulator to replace `LFSR-based` random replacement with `FIFO` to reduce miss spikes.
2. Improve matrix transpose and matrix multiplication performance through loop tiling, register reuse, and better access patterns to reduce cache misses.

Also includes:
- Cache miss classification (compulsory vs. capacity vs. conflict)
- Justification of design trade-offs between LFSR and FIFO
- Evaluation scripts using Spike simulator with cycle modeling

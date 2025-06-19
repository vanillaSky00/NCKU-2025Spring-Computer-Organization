

### Key insights
| Topic                                     | Key idea                                                                                                                                                                                                                                                                                                          | 
| ----------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | 
| **How `merge` works**                     | Maintain two cursors (`a`, `b`) on the already-sorted sub-lists, repeatedly take the smaller current node, splice it to the output list, advance that cursor, stop when one list becomes `NULL`. Complexity = O(n).                                                                                               |                |
| **Why the `next` pointer sits at byte 8** | `struct Node {int data; struct Node* next;}` on RV64 = 4 bytes for `int`, **4 bytes padding** (to keep the 8-byte pointer aligned), then the 8-byte pointer — so offset 8. The grader even hints that Block C is “related to C memory alignment and the fact that the simulator uses RISC-V 64-bit architecture”. |                |
| **Structure alignment**                   | General rule: every field starts at an address that’s a multiple of its size or its requested alignment; the overall struct size is padded to the max alignment used. Alignment prevents split loads/stores and keeps pointer dereference single-cycle.                                                           |                |


### Jumping Backward and Forward in Inline Assembly
Use j 1b instead of j 1 to avoid label conflicts.
```
1: — Defines a local label.

1b — Jumps back to the most recent 1: before this line.

1f — Jumps forward to the next 1: after this line.
```
Note: j 1 only works in compiler-generated local assembly (e.g., .L1).
In inline assembly, labels like 1: aren't globally unique, so always use 1b or 1f for reliable control flow.

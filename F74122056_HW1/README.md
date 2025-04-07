### HW1 - Bubble Sort in Array/ Merge Sort in Linked List
The goal is to integrate assembly instructions directly within C code to handle the sorting operations at a low level, providing more control over the underlying hardware.

### Bubble Sort
I initially started by implementing the loop using inline assembly to perform comparisons and swaps.

I used a "done" flag section that would be triggered once the array is sorted, allowing the loop to exit early.

To ensure better readability, I separated the loop section and done flag section, using an unconditional jump to the appropriate location based on conditions. This made the control flow cleaner and more modular.

### Merge Sort

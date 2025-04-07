#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
    asm volatile(
        /*
        Block A (splitList), which splits the linked list into two halves
        */
        /**
         * slow: iterate 1 time to know the length 
         * faster: use fast/slow pointer
         */
        "add t0, %[head], x0         \n\t"//t0 = address of head
        "addi t1, x0, 1              \n\t"//t1 = 0 = counter for loop

        "COUNT_LEN:                  \n\t"
        "beq t0, x0, DONE_COUNT      \n\t"
        "lw t2, 8(t0)                \n\t"//t2 = cur->next
        "addi t1, t1, 1              \n\t"//get the len
        "add t0, t2, x0"//cur = cur->next
        "j COUNT_LEN"

        "DONE_COUNT:"

        "srli t2, t1, 1" //t2 = t1/2 = len/2

        "add t0, %[head], x0"//reset
        "add t1, x0, 0"
    
        /////i am writing till heree///
        "SPLIT:"
        "add t0, t0, 8"
        "addi t1, t1, 1"
        "bne t1, t2, SPLIT"

        "DONE:"
        "sw t1, %[second]"
        "sw head, %[first]"

        : [first] "=r"(*firstHalf), [second] "=r"(*secondHalf)
        : [head] "r"(head)
        : "t0, t1, t2"
        "");
}

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;

    asm volatile(
        /*
        Block B (mergeSortedList), which merges two sorted lists into one
        */
        "");

    return result;
}

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node

    Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf,
              &secondHalf); // Split the list into two sublists

    firstHalf = mergeSort(firstHalf);   // Recursively sort the left half
    secondHalf = mergeSort(secondHalf); // Recursively sort the right half

    return mergeSortedLists(firstHalf, secondHalf); // Merge the sorted sublists
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int list_size;
    fscanf(input, "%d", &list_size);
    Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
    Node *cur = head;
    for (int i = 0; i < list_size; i++) {
        fscanf(input, "%d", &(cur->data));
        if (i + 1 < list_size)
            cur->next = (Node *)malloc(sizeof(Node));
        cur = cur->next;
    }
    fclose(input);

    // Linked list sort
    head = mergeSort(head);

    cur = head;

    while (cur) {
        printf("%d ", cur->data);
        asm volatile(
            /*
            Block C (Move to the next node), which updates the pointer to
            traverse the linked list
            */
           "addi %[cur], %[cur], 8" //in x86 the offset is 8
           
           : [cur] "+r"(cur)
           : 
           : "memory"//why?
            "");
    }

    printf("\n");
    return 0;
}

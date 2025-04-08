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
        "add t1, %[head], x0         \n\t"

        "1:                          \n\t"
        "ld t1, 8(t1)                \n\t" //fast_p = head->next
        "beqz t1, 2                  \n\t"
        "ld t1, 8(t1)                \n\t" //fast_p = head->next->next
        "beqz t1, 2                  \n\t"
        "ld t0, 8(t0)                \n\t" //slow_p = head->next
        "j 1b                        \n\t"

        "2:                          \n\t"
        "ld t2, 8(t0)                \n\t" //t2=slow->next
        "sd x0, 8(t0)                \n\t" //slow->next = NULL

        "sd %[head], 0(%[first])     \n\t" // *firstHalf = head
        "sd t2, 0(%[second])         \n\t" // *secondHalf = t2
        : 
        : [first] "r" (*firstHalf), [second] "r" (*secondHalf), [head] "r" (head)
        : "t0", "t1", "t2"
        );
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
        "add t3, %[a], 0             \n\t" //address i for a 
        "add t4, %[b], 0             \n\t" //address j for b
        "add t5, %[res], 0           \n\t" //k for res->-..>..

        "comp_loop:                  \n\t"
        "beq t3, x0, b_remain        \n\t"
        "beq t4, x0, a_remain        \n\t"
        "ld t1, 0(t3)                \n\t"
        "ld t2, 0(t4)                \n\t"

        "blt t1, t2, left_part       \n\t"
        "j right_part                \n\t"

        "j done                      \n\t"

        "left_part:                  \n\t"
        "sd t3, 8(t5)                \n\t"
        "ld t3, 8(t3)                \n\t"
        "j update_res                \n\t"

        "right_part:                 \n\t"
        "sd t4, 8(t5)                \n\t"
        "ld t4, 8(t4)                \n\t"

        "update_res:                 \n\t"
        "addi t5, t5, 8              \n\t"//t5 is pointer to res, not a struct
        "j comp_loop                 \n\t"

        "a_remain:                   \n\t"
        "sd t3, 8(t5)                \n\t"
        "j done                      \n\t"

        "b_remain:                   \n\t"
        "sd t4, 8(t5)                \n\t"

        "done:                       \n\t"


        : [res] "+r" (result)
        : [a] "r" (a), [b] "r" (b)
        : "t1", "t2", "t3", "t4", "t5"
        ); 

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
           "ld %[cur], 8(%[cur])       \n\t" //in x86 the offset is 8
           
           : [cur] "+r" (cur)
           : 
           : 
           );
    }

    printf("\n");
    return 0;
}

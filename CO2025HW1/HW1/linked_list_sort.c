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
        "ld t1, 8(t1)                \n\t"//fast_p = head->next
        "beqz t1, 2f                 \n\t"
        "ld t1, 8(t1)                \n\t"//fast_p = head->next->next
        "beqz t1, 2f                 \n\t"
        "ld t0, 8(t0)                \n\t"//slow_p = head->next
        "j 1b                        \n\t"
    
        "2:                          \n\t"
        "ld t2, 8(t0)                \n\t"//t2=slow->next
        "sd x0, 8(t0)                \n\t"//slow->next = NULL

        "sd %[head], 0(%[first])     \n\t"//*firstHalf = head
        "sd t2, 0(%[second])         \n\t"
	:
        : [first] "r" (firstHalf), [second] "r" (secondHalf), [head] "r" (head)
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
	"beqz %[a], merge_b_remain    \n\t"//if a is NULL, merge remaining b
        "beqz %[b], merge_a_remain    \n\t"//if b is NULL, merge remaining a
        
        // initialize result with the smaller first element
        "ld t0, 0(%[a])               \n\t"//t0 = a->data
        "ld t1, 0(%[b])               \n\t"//t1 = b->data
        "bge t0, t1, merge_b_first    \n\t"//if a->data >= b->data, merge b
        
        "merge_a_first:               \n\t"
        "add %[res], %[a], x0         \n\t"//result = a
        "add %[tail], %[a], x0        \n\t"//tail = a
        "ld %[a], 8(%[a])             \n\t"//a = a->next
        "j merge_compare              \n\t"
        
        "merge_b_first:               \n\t"
        "add %[res], %[b], x0         \n\t"//result = b
        "add %[tail], %[b], x0        \n\t"//tail = b
        "ld %[b], 8(%[b])             \n\t"//b = b->next
        
        "merge_compare:               \n\t"
        "beqz %[a], merge_b_remain    \n\t"//if a is NULL, merge remaining b
        "beqz %[b], merge_a_remain    \n\t"//if b is NULL, merge remaining a
        
        "ld t0, 0(%[a])               \n\t"//t0 = a->data
        "ld t1, 0(%[b])               \n\t"//t1 = b->data
        "bge t0, t1, merge_b_next     \n\t"//if a->data >= b->data, take b
        
        "merge_a_next:                \n\t"
        "sd %[a], 8(%[tail])          \n\t"//tail->next = a
        "add %[tail], %[a], x0        \n\t"//tail = a
        "ld %[a], 8(%[a])             \n\t"//a = a->next
        "j merge_compare              \n\t"
        
        "merge_b_next:                \n\t"
        "sd %[b], 8(%[tail])          \n\t"//tail->next = b
        "add %[tail], %[b], x0        \n\t"//tail = b
        "ld %[b], 8(%[b])             \n\t"//b = b->next
        "j merge_compare              \n\t"
        
        "merge_a_remain:              \n\t"
        "beqz %[a], merge_done        \n\t"//if a is NULL, done
        "sd %[a], 8(%[tail])          \n\t"//tail->next = a
        "j merge_done                 \n\t"
        
        "merge_b_remain:              \n\t"
        "beqz %[b], merge_done        \n\t"//if b is NULL, done
        "sd %[b], 8(%[tail])          \n\t"//tail->next = b
        
        "merge_done:                  \n\t"
        : [res] "+r" (result), [tail] "+r" (tail), [a] "+r" (a), [b] "+r" (b)
        : 
        : "t0", "t1"
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
            
	   "ld %[cur], 8(%[cur])      \n\t" //in x86 the offset is 8
           
           : [cur] "+r" (cur)
           :
           : 
	   );
    }
    printf("\n");
    return 0;
}

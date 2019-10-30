// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <console.h>

#include <memoryManager.h>

typedef union mem_node {
    struct {
        /* Pointers to the next and previous memory block nodes */
        union mem_node * next;
        union mem_node * prev;

        /* Pointer to start of the block (node) */
        uint8_t * address;

        /* Quantity of consecutive free blocks */
        uint64_t size;
    } n;
    long x;
} node;


typedef struct mem_header {
    node * freeList;
    node * usedList;
 
    /* Measured in blocks */
    uint64_t usedBlocks;
    uint64_t freeBlocks;

    /* Block size in bytes */
    uint64_t blockSize;
} header;

/* Statics functions */
static void extract_node(node * n, node ** list);
static void insert_node(node * n, node ** list);
static void subdivide_node(node * n, uint64_t size);
static void merge_node(node * n);

/* Header of the memory manager */
static header memory;

/* Memory Manager builder */
void create_manager(uint8_t * address, uint64_t totalBytes) {    
    /* Initialize list header */
    memory.blockSize = sizeof(node) * 2;
    memory.freeBlocks = totalBytes / memory.blockSize;
    memory.usedBlocks = 0;
    memory.freeList = (node *) address;
    memory.usedList = 0;

    /* Create first block of totalBytes bytes */
    node first;
    first.n.next = 0;
    first.n.prev = 0;
    first.n.address = address;
    first.n.size = memory.freeBlocks;
    memcpy(address, &first, sizeof(node));
}

/* Reserves bytes space on memory */
void * malloc(uint64_t bytes) {
    /* Quantity of the required blocks */
    uint64_t requiredBlocks = (bytes + sizeof(node) - 1) / memory.blockSize + 1;
    
    /* No available space */
    if (requiredBlocks > memory.freeBlocks) return 0;

    /* Search for node with enough space */
    node * iterator = memory.freeList;
    while (iterator != 0 && iterator->n.size < requiredBlocks)
        iterator = iterator->n.next;
    
    /* Not enough consecutive blocks */
    if (iterator == 0) return 0;

    node * found = iterator;

    /* Subdivide node if needed */
    subdivide_node(found, requiredBlocks);
    
    /* Update free list (extract found node from it) */
    extract_node(found, &memory.freeList);

    /* Update used list (insert found node on it) */
    insert_node(found, &memory.usedList);
    
    /* Update memory data */
    memory.usedBlocks += requiredBlocks;
    memory.freeBlocks -= requiredBlocks;

    /* Returns extracted node */
    return (void *) (found->n.address + sizeof(node));
}

/* Frees space on memory */
void free(void * ptr) {
    /* Creates a pointer to the real start of the block */
    ptr = (uint8_t *)ptr - sizeof(node);
    
    /* Search for the pointer */
    node * iterator = memory.usedList;
    while (iterator != 0 && iterator->n.address < ptr)
        iterator = iterator->n.next;
        
    /* If not found */
    if (iterator == 0 || iterator->n.address > ptr) return;

    node * found = iterator;

    /* Updates used list (extract found node from it) */
    extract_node(found, &memory.usedList);

    /* Update free list (insert found node on it) */
    insert_node(found, &memory.freeList);

    /* Merges node on free list if needed */
    merge_node(found);

    /* Updates header values */
    memory.usedBlocks -= found->n.size;
    memory.freeBlocks += found->n.size;
}

/* Gets memory status */
void status(uint64_t * total, uint64_t * occupied, uint64_t * free) {
    *total = (memory.freeBlocks + memory.usedBlocks) * memory.blockSize;
    *occupied = memory.usedBlocks * memory.blockSize;
    *free = memory.freeBlocks * memory.blockSize;
}

/* Prints memory status */
void mm_print_status() {
    uint64_t occupied = memory.usedBlocks * memory.blockSize;
    uint64_t freed = memory.freeBlocks * memory.blockSize;
    uint64_t total = occupied + freed;

    print("\n----- Estado de la memoria -----");
    print("\nTotal Size: %d\nOcuppied Size: %d\nFree Size: %d", total, occupied, freed);
}

/* Returns the first address from the next block, 
** assuming ptr is a valid return from malloc */
void * get_last_address (void * ptr) {
    node * aux = (node *) ((uint8_t *) ptr - sizeof(node));
    return (void *)(aux->n.address + aux->n.size * memory.blockSize);
}

/* Prints state of the given node list */
static void print_list_state(node * iterator) {
    while (iterator != 0) {
        print("- Size: %d", iterator->n.size);
        print("- Address: 0x");
        printHex((uint64_t)iterator->n.address);
        print("\t to Last: 0x");
        printHex((uint64_t)get_last_address(iterator->n.address + sizeof(node)));
        print("\n");
        iterator = iterator->n.next;
    }
    print("\n---------------\n: ");
}

/* Prints memory state */
void mm_print_state() {
    print("\nFree list: \n");
    print_list_state(memory.freeList);
    print("\nUsed list: \n");
    print_list_state(memory.usedList);
}

/* Extracts de given node from the given double linked list */
static void extract_node(node * n, node ** list) {
    /* First node */
    if (n->n.prev == 0) {
        *list = n->n.next;
        if (*list != 0) (*list)->n.prev = 0;
        return;
    }

    /* Middle or last node */
    n->n.prev->n.next = n->n.next;
    if (n->n.next != 0) n->n.next->n.prev = n->n.prev;
}

/* Inserts the given node (ordered by address) onto the given double linked list */
static void insert_node(node * n, node ** list) {
    /* Search the corect position to insert given node */
    node * iterator = *list;
    node * prev;
    while (iterator != 0 && iterator->n.address < n->n.address) {
        prev = iterator;
        iterator = iterator->n.next;
    }
    
    /* First (could be only one) */
    if (iterator == *list) {
        n->n.next = *list;
        n->n.prev = 0;
        *list = n;
        if (n->n.next != 0) n->n.next->n.prev = n;
        return;
    }

    /* Middle or last (insert between prev and iterator) */
    n->n.prev = prev;
    n->n.next = iterator;
    prev->n.next = n;
    if (iterator != 0) iterator->n.prev = n;
}

/* Subdivides node n in two bocks, one with size, and the other with remaining data */
static void subdivide_node(node * n, uint64_t size) {
    if (n->n.size == size) return;
    
    /* Creates a new node next to node given */
    node newNode;
    newNode.n.size = n->n.size - size;
    newNode.n.next = n->n.next;
    newNode.n.prev = n;
    newNode.n.address = n->n.address + size * memory.blockSize;
    memcpy(newNode.n.address, &newNode, sizeof(node));

    /* Updates the node given */
    n->n.size = size;
    n->n.next = newNode.n.address;
} 

/* Merges the block with the next one (if free) */
static void merge_next(node * n) {
    node * nextBlock = n->n.next;
    if (nextBlock == 0 || n->n.address + n->n.size * memory.blockSize != nextBlock->n.address)
        return;

    n->n.size += nextBlock->n.size;
    n->n.next = nextBlock->n.next;
    if (n->n.next != 0) n->n.next->n.prev = n;        
}

/* Merges the block with the next one and / or previous one (if free) */
static void merge_node(node * n) {
    merge_next(n);
    if (n->n.prev != 0) merge_next(n->n.prev);
}
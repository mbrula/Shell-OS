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
    uint64_t occupied;
    uint64_t free;

    /* Block size in bytes */
    uint64_t blockSize;
} header;

/* Static function to help merging */
static void merge_next(node * block);

/* Header of the memory manager */
static header memory;

/* Memory Manager builder */
void create_manager(uint8_t * address, uint64_t blockSize, uint64_t totalBytes) {    
    /* Initialize list header */
    memory.free = totalBytes / (blockSize + sizeof(node));
    memory.occupied = 0;
    memory.freeList = (node *) address;
    memory.usedList = 0;
    memory.blockSize = blockSize;

    /* Create first block of maxPages pages */
    node first;
    first.n.next = 0;
    first.n.prev = 0;
    first.n.address = address;
    first.n.size = totalBytes / blockSize;

    memcpy(address, &first, sizeof(node));
}

/* Reserves bytes space on memory */
void * malloc(uint64_t bytes) {
    /* Quantity of the required blocks */
    uint64_t pageCount = (bytes + sizeof(node)) / memory.blockSize + 1;
    
    /* No available space */
    if (pageCount > memory.free) return 0;

    /* Search for node with enough space */
    node * found = memory.freeList;
    while (found != 0 && found->n.size < pageCount) {
        found = found->n.next;
    }
    /* Not enough space */
    if (found == 0) return 0;

    /* If we have more space than required */
    if (found->n.size > pageCount) {
        /* Create new node pointing to start of the next block, 
        ** and conect we connect it */
        node newNode;
        newNode.n.prev = found->n.prev;
        newNode.n.next = found->n.next;
        newNode.n.address = found->n.address + pageCount * memory.blockSize;
        newNode.n.size = found->n.size - pageCount;
        memcpy(newNode.n.address, &newNode, sizeof(node));
        if (found->n.prev == 0)
            memory.freeList = (node *) newNode.n.address;
        else
            found->n.prev->n.next = (node *) newNode.n.address;
    } else {
        if (found->n.prev == 0)
            memory.freeList = found->n.next;
        else
            found->n.prev->n.next = found->n.next;
    }

    /* Update properties of the extrcted node and add to usedList */
    found->n.size = pageCount;
    found->n.prev = 0;
    found->n.next = memory.usedList;
    memory.usedList = found;
    
    /* Update memory data */
    memory.occupied += pageCount;
    memory.free -= pageCount;

    // print("\tDirec Mal: 0x");
    // printHex((uint64_t) found->n.address);

    /* Returns extracted node */
    return (void *) (found->n.address + sizeof(node));
}

/* Frees space on memory */
void free(void * ptr) {
    /* SEARCH of the ptr on used list */
    /* Creates a pointer to the real start of the block */
    uint8_t * pointer = (uint8_t *)ptr - sizeof(node);

    // print("\tDirec Free: 0x");
    // printHex((uint64_t) pointer);

    node * iterator = memory.usedList;
    node * prev = iterator;
    
    /* Search for the pointer */
    while (iterator != 0 && iterator->n.address != pointer) {
        prev = iterator;
        iterator = iterator->n.next;
    }
        
    /* If not found */
    if (iterator == 0) return;
    node * blockToFree = iterator;

    /* Updates used list */
    if (memory.usedList == blockToFree) memory.usedList = blockToFree->n.next;
    else prev->n.next = blockToFree->n.next;

    /* SEARCH found block on free list */
    iterator = memory.freeList;
    prev = iterator;

    /* Searches the correct position to insert */
    while (iterator != 0 && iterator->n.address < blockToFree->n.address) {
        prev = iterator;
        iterator = iterator->n.next;
    }
    
    /* Inserts between node prev and iterator */
    /* First place */
    if (prev == iterator) {
        memory.freeList = blockToFree;
        blockToFree->n.prev = 0;
        blockToFree->n.next = iterator;
    } else {
        blockToFree->n.prev = prev;
        blockToFree->n.next = prev->n.next;
        prev->n.next = blockToFree;
        if (iterator != 0) iterator->n.prev = blockToFree;
    } 

    /* Updates header values */
    memory.occupied -= blockToFree->n.size;
    memory.free += blockToFree->n.size;

    /* Checks for a next contiguous free block and merges it */
    merge_next(blockToFree);
    if (blockToFree->n.prev != 0) merge_next(blockToFree->n.prev);
}

/* Gets memory status */
void status(uint64_t * total, uint64_t * occupied, uint64_t * free) {
    *total = (memory.free + memory.occupied) * memory.blockSize;
    *occupied = memory.occupied * memory.blockSize;
    *free = memory.free * memory.blockSize;
}

/* Prints memory status */
void printStatus() {
    uint64_t occupied = memory.occupied * memory.blockSize;
    uint64_t freed = memory.free * memory.blockSize;
    uint64_t total = occupied + freed;

    print("\n----- Estado de la memoria -----");
    print("\nTotal Size: %d\nOcuppied Size: %d\nFree Size: %d", total, occupied, freed);
}

/* Merges the block with the next one (if free) */
static void merge_next(node * block) {
    node * nextBlock = block->n.next;
    if (nextBlock != 0 && block->n.address + block->n.size * memory.blockSize == nextBlock->n.address) {
        block->n.size += nextBlock->n.size;
        block->n.next = nextBlock->n.next;
    }
}

/* Returns the first address from the next block, 
** assuming ptr is a valid return from malloc */
void * getLastAddress (void * ptr) {
    node * aux = (node *) ((uint8_t *) ptr - sizeof(node));
    return aux->n.address + aux->n.size * memory.blockSize;
}

/* Prints memory state */
void printMemState() {
    // node * freed = memory.freeList;
    // print("\nLista de frees: \n");
    // while (freed != 0) {
    //     print("- Size: %d", freed->n.size);
    //     print("- Address: 0x");
    //     printHex((uint64_t)freed->n.address);
    //     print("\t to Last: 0x");
    //     printHex((uint64_t)getLastAddress(freed->n.address + sizeof(node)));
    //     print("\n");
    //     freed = freed->n.next;
    // }
    // print("\n---------------\n: ");

    // node * used = memory.usedList;
    // print("\nLista de used: \n");
    // while (used != 0) {
    //     print("- Size: %d", used->n.size);
    //     print("- Address: 0x");
    //     printHex((uint64_t)used->n.address);
    //     print("\n");
    //     used = used->n.next;
    // }
    // print("\n---------------\n: ");
    print("\n----- Memory Manager -----\n");
    print("Node size: 0x");
    printHex(sizeof(node));
    print("\nFree memory blocks: 0x");
    printHex(memory.free);
}
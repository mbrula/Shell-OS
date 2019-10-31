// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <console.h>

#include <memoryManager.h>

#ifdef BUDDY
/*********************************************
**  Start of the Free List Memory Manager   **
**********************************************/

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
    while (iterator != 0 && iterator->n.address < (uint8_t) ptr)
        iterator = iterator->n.next;
        
    /* If not found */
    if (iterator == 0 || iterator->n.address > (uint8_t) ptr) return;

    node * found = iterator;

    /* Updates used list (extract found node from it) */
    extract_node(found, &memory.usedList);

    /* Update free list (insert found node on it) */
    insert_node(found, &memory.freeList);

    /* Updates header values */
    memory.usedBlocks -= found->n.size;
    memory.freeBlocks += found->n.size;

    /* Merges node on free list if needed */
    merge_node(found);
}

/* Gets memory status */
void mm_status(uint64_t * total, uint64_t * occupied, uint64_t * free) {
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
    if (iterator == 0) print("\n");
    uint64_t num = 0;
    while (iterator != 0) {
        print(" Num: %d - Blocks: %d", num++, iterator->n.size);
        print(" - Address: 0x");
        printHex((uint64_t)iterator->n.address);
        print(" to 0x");
        printHex((uint64_t)get_last_address(iterator->n.address + sizeof(node)));
        
        /* This prints are for testing porpuse 
        print(" - Previous: 0x");
        printHex((uint64_t)iterator->n.prev);
        print(" - Next: 0x");
        printHex((uint64_t)iterator->n.next); */
        
        print("\n");
        iterator = iterator->n.next;
    }
    print("\n");
}

/* Prints memory state */
void mm_print_state() {
    print("\n ----- Memory status -----\n");
    print("\n - Free list:\n");
    print_list_state(memory.freeList);
    print("\n - Used list:\n");
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
    if (newNode.n.next != 0) newNode.n.next->n.prev = (node *) newNode.n.address;
    memcpy(newNode.n.address, &newNode, sizeof(node));

    /* Updates the node given */
    n->n.size = size;
    n->n.next = (node *) newNode.n.address;
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

#else
/*****************************************
**  Start of the Buddy Memory Manager   **
******************************************/

/* Posibles states of a node */
typedef enum {FREE = 0, USED, PUSED} nodeState;

typedef union mem_tree_node {
    struct {
        /* Pointers left and right nodes */
        union mem_tree_node * left;
        union mem_tree_node * right;

        /* State of the current node */
        nodeState state;

        /* Pointer to start of the block (node) */
        uint8_t * address;

        /* Level, exponent of base 2 */
        uint64_t level;   
    } n;
    long x;
} node;

typedef struct mem_header {
    uint8_t * address;

    /* Where the nodes are stored */
    node * nodeList;
 
    /* Measured in blocks */
    uint64_t usedBlocks;
    uint64_t freeBlocks;

    /* Block size in bytes */
    uint64_t blockSize;

    /* Block size in level (exp 2) */
    uint8_t maxLevel;
    uint8_t minLevel;
} header;

/* Static auxiliary functions */
static void init_header(uint8_t * address, uint64_t totalBytes);
static void init_nodes(uint64_t index, uint8_t * address, uint64_t level);
static void * find_block(node * n, uint64_t level);
static uint64_t delete_block(node * n, uint8_t * ptr);
static node * get_node(node * n, uint8_t * ptr);
static void printNodes(node * list);

/* Header of the memory manager */
static header memory;

/* Memory Manager builder (totalbytes is assumed to be exp 2) */
void create_manager(uint8_t * address, uint64_t totalBytes) {    
    init_header(address, totalBytes);
    init_nodes(0, memory.address, memory.maxLevel);
}

/* Reserves bytes space on memory */
void * malloc(uint64_t bytes) {
    return find_block(memory.nodeList, max(exp2(bytes), memory.minLevel));   
}

/* Frees space on memory */
void free(void * ptr) {
    if (ptr == 0) return;
    delete_block(memory.nodeList, (uint8_t *) ptr); 
}

/* Gets memory status */
void mm_status(uint64_t * total, uint64_t * occupied, uint64_t * free) {
    *occupied = memory.usedBlocks * memory.blockSize;
    *free = memory.freeBlocks * memory.blockSize;
    *total = *occupied + *free;
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
void * get_last_address(void * ptr) {
    node * n = get_node(memory.nodeList, (uint8_t *) ptr);
    return n->n.address + pow(2, n->n.level);
}

/* Prints memory state */
void mm_print_state() {
    printNodes(memory.nodeList);
}


/* Initializes al the data for the header (total bytes expected in base 2) */
static void init_header(uint8_t * address, uint64_t totalBytes) {
    /* Staring max level, min level and block size */
    memory.blockSize = sizeof(node);
    memory.maxLevel = exp2(totalBytes);
    memory.minLevel = exp2(memory.blockSize);

    /* Initial configuration */
    uint64_t realBytes = totalBytes;
    uint64_t nodeSpace = totalBytes - realBytes;
    uint64_t maxBlocks = pow(2, memory.maxLevel - memory.minLevel + 1) - 1;

    /* Searches for the best configuration posible */
    while (maxBlocks * sizeof(node) > nodeSpace) {
        memory.maxLevel -= 1;
        realBytes /= 2;
        nodeSpace = totalBytes - realBytes;
        maxBlocks = pow(2, memory.maxLevel - memory.minLevel + 1) - 1;
    }

    /*  Finally initializes remaining memory data */
    memory.address = address + nodeSpace;
    memory.nodeList = (node *) address; 
    memory.freeBlocks = pow(2, memory.maxLevel - memory.minLevel);
    memory.usedBlocks = 0;
}

/* Initializes vector of nodes */
static void init_nodes(uint64_t index, uint8_t * address, uint64_t level) {
    /* Initialize current node */
    memory.nodeList[index].n.address = address;
    memory.nodeList[index].n.level = level;
    memory.nodeList[index].n.state = FREE;

    /* If final level */
    if (level == memory.minLevel) {
        memory.nodeList[index].n.left = 0;
        memory.nodeList[index].n.right = 0;
        return;
    }

    memory.nodeList[index].n.left = memory.nodeList + index + 1;
    memory.nodeList[index].n.right = memory.nodeList + index + pow(2, level - memory.minLevel);
    
    /* Initialize left and right part of the tree */
    init_nodes(index + 1, address, level - 1);
    init_nodes(index + pow(2, level - memory.minLevel), address + pow(2, level - 1), level - 1);
}

/* Recursive function, creates children if needed, returns node direction */
static void * find_block(node * n, uint64_t level) {
    if (n->n.level == level) {
        if (n->n.state != FREE) return 0;
        else {
            n->n.state = USED;
            uint64_t aux = pow(2, n->n.level - memory.minLevel);
            memory.freeBlocks -= aux;
            memory.usedBlocks += aux;
            return (void *) n->n.address;
        }
    }
    
    /* It has a superior level but used */
    if (n->n.state == USED) return 0;

    /* Superior level and free or partially occupied */
    /* If its FREE we will find place for block, so is PO */
    n->n.state = PUSED;
    void * aux = find_block(n->n.left, level);
    if (aux == 0) aux = find_block(n->n.right, level);   

    /* Updates state of the current node (at this point it was PO) */
    if (n->n.left->n.state == USED && n->n.right->n.state == USED) n->n.state = USED;

    return aux;
}

/* Merges childs of the given node */
static void merge_childs(node * n) {
    n->n.state = PUSED;
    /* If childs are both free */
    if (n->n.left->n.state == FREE && n->n.right->n.state == FREE) n->n.state = FREE;
}

/* Recursive function to free a block */
static uint64_t delete_block(node * n, uint8_t * ptr) {
    /* If smaller addres or the block is free, return */
    if (n->n.address > ptr || n->n.state == FREE)
        return 0;

    /* Same address and min level or both childs free, then found */
    if (n->n.address == ptr && (n->n.level == memory.minLevel || (n->n.left->n.state == FREE && n->n.right->n.state == FREE))) {
        n->n.state = FREE;
        uint64_t aux = pow(2, n->n.level - memory.minLevel);
        memory.freeBlocks += aux;
        memory.usedBlocks -= aux;
        return 1;
    }

    /* If min level, stop searching */
    if (n->n.level == memory.minLevel) return 0;

    /* Recursive search on childs */
    uint64_t aux = delete_block(n->n.left, ptr);
    if (aux == 0) aux = delete_block(n->n.right, ptr);

    /* If found, update state and potentially merge */
    if (aux == 1) merge_childs(n);
    return aux;

}

/* Recursive function to get a node given a pointer */
static node * get_node(node * n, uint8_t * ptr) {
    /* If smaller addres or the block is free, return */
    if (n->n.address > ptr || n->n.state == FREE)
        return 0;

    /* Same address and min level or both childs free, then found */
    if (n->n.address == ptr && (n->n.level == memory.minLevel || (n->n.left->n.state == FREE && n->n.right->n.state == FREE)))
        return n;

    /* If min level, stop searching */
    if (n->n.level == memory.minLevel) return 0;

    /* Recursive search on childs */
    node * aux = delete_block(n->n.left, ptr);
    if (aux == 0) aux = delete_block(n->n.right, ptr);
    return aux;
}

/* Prints all nodes in the order they are stored */
static void printNodes(node * list) {
    uint64_t final = pow(2, memory.maxLevel - memory.minLevel + 1) - 1;
    for (uint64_t i = 0; i < final; i++) {
        print("Index: %d - State: %d - Level: %d - Address: 0x", i, list[i].n.state, list[i].n.level);
        printHex((uint64_t)list[i].n.address);
        print("\n");
    }
}

#endif
#ifndef LCACHE_H
#define LCACHE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Node Node;
typedef struct Cache Cache;
typedef struct HashNode HashNode;

struct Node {
    /* using string for key
    * integer for data */
    char *key;
    int data;

    /* nodes to visit/track
    *  next and previous nodes
    *  to perform operations */
    Node *next;
    Node *prev;
};

struct HashNode {
    /* HashTable has its own hashNodes
    *  which stores the pointer to nodes */
    // key is only storing pointers
    char *key;
    Node *saved_node;

    // reference to next HashNode
    HashNode *next;
};

// Two unsigned long prime numbers for hashing 
#define M 1000000009ULL
#define P 31ULL

// The actual cache
struct Cache {
    //used haid and tail for envict and insert
    Node *head, *tail;

    /* capacity which user provides while creating
     * list, and added size variable to check if 
     * cache is full */
    size_t capacity;
    size_t size;

    // used double pointers to save the nodes.
    HashNode **HashTable;
};

//Definition of all basic and helper functions
uint64_t hash (const char *key, uint64_t size); //done
Node *hash_search (Cache *c, const char *key); //done
void hash_insert (Cache *c, Node *node); //done
void hash_delete (Cache *c, const char *key); //done
Cache *init_cache (size_t capacity); //done
void move_to_head (Cache *c, Node *node); //done
void evict_tail (Cache *c); //done
void insert_head (Cache *c, Node *node); //done
Node *create_node (const char *key, int data); //done 
void delete_node (Cache *c, Node *nodeToDelete); //done
int get (Cache *c, const char *key); //done
void put (Cache *c, const char *key, int data); // done
void print_cache (Cache *c); //done
void destroy_cache (Cache **c); //done

#endif // LCACHE_H

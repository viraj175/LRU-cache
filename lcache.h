#ifndef LCACHE_H
#define LCACHE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Node Node;
typedef struct Cache Cache;
typedef struct HashNode HashNode;

struct Node {
    char *key;
    int data;

    Node *next;
    Node *prev;
};

struct HashNode {
    char *key;
    Node *first_node;
    HashNode *next;
};

#define M 1000000009ULL
#define P 31ULL

struct Cache {
    Node *head, *tail;
    size_t capacity;
    size_t size;
    HashNode **HashTable;
};

uint64_t hash (const char *data, uint64_t size); //done
Node *hash_search (Cache *c, const char *data); //done
void hash_insert (Cache *c, Node *node); //done
void hash_delete (Cache *c, const char *key); //done
Cache *init_cache (size_t capacity); //done
void move_to_head (Cache *c, Node *node); 
void evict_tail (Cache *c);
void insert_head (Cache *c, Node *node);
Node *create_node (const char *key, int data); //done 
void delete_node (Cache *c, Node *nodeToDelete); //done
Node *get (Cache *c);
void put (Cache *c, const char *key, int data); // done
void print_cache (Cache *c); //done
void destroy_cache (Cache **c); //update needed

#endif // LCACHE_H

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
    Node *cache_node;
    HashNode *next;
};

#define M 1000000009ULL
#define P 31ULL

struct Cache {
    Node *head, *tail;
    size_t capacity;
    HashNode **HashTable;
};

uint64_t hash (const char *data, uint64_t size); //done
Node *hashSearch (Cache *c, const char *data); //done
void hashInsert (Cache *c, Node *node); //done
Cache *init_cache (size_t capacity); //done
Node *createNode (const char *key, int data); //done 
void deleteNode (Cache *c, const char *key); //done
Node *get (Cache *c);
void put (Cache *c, const char *key, int data); // done
void print_cache (Cache *c); //done
void destroy_cache (Cache **c); //update needed

#endif // LCACHE_H

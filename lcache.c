#include "lcache.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>


uint64_t hash(const char *data, uint64_t size)
{
    uint64_t result = 0;
    uint64_t p_pow = 1;
    for (uint64_t i = 0; data[i] != '\0'; ++i) 
    {
        result = (result + (data[i] * p_pow) % M) % M;
        p_pow = (p_pow * P) % M;
    }

    return (result % M) % size;
}

Cache *init_cache(size_t capacity)
{
    Cache *c = malloc(sizeof(Cache));
    if (c == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n"); 
        return NULL;
    }

    c->capacity = capacity;
    c->head = c->tail = NULL;

    c->HashTable = calloc(capacity, sizeof(HashNode *));
    if (c->HashTable == NULL) 
    {
        fprintf(stderr, "Memory allocation failed!\n"); 
        return NULL;
    }
    
    return c;
}

void destroy_cache(Cache **c)
{
    if (*c == NULL) return;

    Node *delete = (*c)->head;
    while (delete != NULL)
    {
        Node *temp = delete->next;
        SAFE_FREE(delete->key);
        SAFE_FREE(delete);
        delete = temp;
    }

    for (size_t i = 0; i < (*c)->capacity; ++i)
    {
        HashNode *hashNodeDelete = (*c)->HashTable[i];
        while (hashNodeDelete != NULL)
        {
            HashNode *temp = hashNodeDelete->next;
            SAFE_FREE(hashNodeDelete);
            hashNodeDelete = temp;
        }
    }
    SAFE_FREE((*c)->HashTable);
    SAFE_FREE(*c);
}

void print_cache(Cache *c)
{
    if (c == NULL) return;

    printf(RED "%p" RESET "<- " YELLOW "HEAD" RESET " ->", NULL);
    Node *temp = c->head;
    while (temp != NULL)
    {
        if (temp->next != NULL) 
            printf(" [ " BLUE "%s" RESET " | " GREEN "%d" RESET " ] <=>", temp->key, temp->data);
        else 
            printf(" [ " BLUE "%s" RESET " | " GREEN "%d" RESET " ] ", temp->key, temp->data);
        temp = temp->next;
    }
    printf("<- " YELLOW "TAIL" RESET " ->" RED "%p" RESET "\n", NULL);
}

Node *hashSearch(Cache *c, const char *key)
{
    uint64_t index = hash(key, c->capacity);
    HashNode *hash_node = c->HashTable[index];

    while (hash_node != NULL)
    {
        if (strcmp(hash_node->cache_node->key, key) == 0) return hash_node->cache_node;
        hash_node = hash_node->next;
    }
    
    return NULL;
}

void hashInsert (Cache *c, Node *node)
{
    uint64_t index = hash(node->key, c->capacity);
    HashNode *temp = c->HashTable[index];

    if (hashSearch(c, node->key))
    {
        LOG_ERROR_MSG("key already exist");
        return;
    }

    if (temp == NULL)
    {
        c->HashTable[index] = malloc(sizeof(HashNode));
        if (c->HashTable[index] == NULL)
        {
            LOG_ERROR_MSG("malloc failed");
            return;
        }
        c->HashTable[index]->cache_node = node;
        c->HashTable[index]->next = NULL;
        return;
    }
    else while (temp->next != NULL) temp = temp->next;
    
    HashNode *add = malloc(sizeof(HashNode));
    if (add == NULL)
    {
        LOG_ERROR_MSG("Malloc failed");
        return;
    }

    add->next = NULL;
    add->cache_node = node;
    temp->next = add;
}

Node *createNode(const char *key, int data)
{
    Node *node = malloc(sizeof(Node));
    node->key = malloc(strlen(key) + 1);
    if (node == NULL)
    {
        LOG_ERROR_MSG("Malloc failed");
        return NULL;
    }
    
    strcpy(node->key, key);
    node->data = data;

    return node;
}

void deleteNode(Cache *c, const char *key)
{
    if (c == NULL) return;
    
    Node *nodeToDelete = hashSearch(c, key);
    if (nodeToDelete == NULL) 
    {
        LOG_ERROR_MSG("invalid node provided");
        return;
    }

    nodeToDelete->prev->next = nodeToDelete->next;
    nodeToDelete->next->prev = nodeToDelete->prev;

    SAFE_FREE(nodeToDelete->key);
    SAFE_FREE(nodeToDelete);
}

void put(Cache *c, const char *key, int data)
{
    if (c == NULL) 
    {
        LOG_ERROR_MSG("Cache doesn't exist");
        return;
    }

    if (c->head == NULL)
    {
        Node *temp = createNode(key, data);
        hashInsert(c, temp);
        c->head = c->tail = temp;
        c->head->prev = c->tail->next = NULL;
        return;
    }

    Node *exist = hashSearch(c, key);

    if (exist)
    {
        exist->data = data;
        exist->prev->next = exist->next;
        exist->next->prev = exist->prev;
        exist->next = c->head;
        c->head->prev = exist;
        c->head = exist;
        c->head->prev = NULL;
        return;
    }
    
    exist = createNode(key, data);
    hashInsert(c, exist);
    exist->next = c->head;
    c->head->prev = exist;
    c->head = exist;
    c->head->prev = NULL;
}

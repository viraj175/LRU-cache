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
    c->size = 0;
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

Node *hash_search(Cache *c, const char *key)
{
    uint64_t index = hash(key, c->capacity);
    HashNode *hash_node = c->HashTable[index];

    while (hash_node != NULL)
    {
        if (strcmp(hash_node->first_node->key, key) == 0) return hash_node->first_node;
        hash_node = hash_node->next;
    }
    
    return NULL;
}

void hash_insert (Cache *c, Node *node)
{
    uint64_t index = hash(node->key, c->capacity);
    HashNode *temp = c->HashTable[index];

    if (hash_search(c, node->key))
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
        c->HashTable[index]->first_node = node;
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
    add->first_node = node;
    temp->next = add;
}

void hash_delete(Cache *c, const char *key)
{
    uint64_t index = hash(key, c->size);
    // Node *node = hash_search(c, key);
    HashNode *del = c->HashTable[index];
    HashNode *prev = NULL;

    while (del->next != NULL) {
        prev = del;
        del = del->next;
    } 

    if (prev == NULL)
    {
        c->HashTable[index] = c->HashTable[index]->next;
    }
    else if (del == NULL)
    {
        prev->next = NULL;
    }
    else 
    {
        prev->next = del->next;
    }

    SAFE_FREE(del);
}

Node *create_node(const char *key, int data)
{
    Node *node = malloc(sizeof(Node));
    node->key = malloc(strlen(key) + 1);
    if (node == NULL || node->key == NULL)
    {
        LOG_ERROR_MSG("Malloc failed");
        return NULL;
    }
    
    strcpy(node->key, key);
    node->data = data;

    return node;
}

void delete_node(Cache *c, Node *del)
{
    
    if (del == NULL || c == NULL) 
    {
        LOG_ERROR_MSG("invalid node provided");
        return;
    }

    if (del == c->head && del == c->tail)
    {
        c->head = c->tail = NULL;
    }
    else if (del == c->head)
    {
        c->head = c->head->next;
        c->head->prev = NULL;
    }
    else if (del == c->tail)
    {
        c->tail = c->tail->prev;
        c->tail->next = NULL;
    }
    else 
    {
        del->prev->next = del->next;
        del->next->prev = del->prev;
    }

    hash_delete(c, del->key);
    SAFE_FREE(del->key);
    SAFE_FREE(del);
}

void insert_head(Cache *c, Node *node)
{
    node->prev = NULL;
    node->next = c->head;

    if (c->head != NULL)
    {
        c->head->prev = node;
    }
    else 
    {
        c->tail = node;
    }
    c->head = node;
}

void move_to_head(Cache *c, Node *node)
{
    if (node == c->head) return;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    else c->tail = node->prev;

    insert_head(c, node);
}

void evict_tail(Cache *c)
{
    delete_node(c, c->tail);
}

void put(Cache *c, const char *key, int data)
{
    if (c == NULL) 
    {
        LOG_ERROR_MSG("Cache doesn't exist");
        return;
    }

    Node *exist = hash_search(c, key);

    if (exist)
    {
        exist->data = data;
        move_to_head(c, exist);
        return;
    }

    if (c->size == c->capacity) 
    {
        evict_tail(c);
    }

    Node *node = create_node(key, data);
    insert_head(c, node);
    hash_insert(c, node);               
    ++c->size;
}

int get (Cache *c, const char *key)
{
    Node *exist = hash_search(c, key);

    if (exist)
    {
        move_to_head(c, exist);
        return exist->data;
    }
    
    return -1;
}

#include "lcache.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>


/* hash function that hash our key used hash method for strings
 * called polynomial rolling hash function to reduce collisons 
 * it takes two argument key and the size of our hash table. */
uint64_t hash(const char *key, uint64_t size)
{
    uint64_t result = 0;
    uint64_t p_pow = 1;
    for (uint64_t i = 0; key[i] != '\0'; ++i) 
    {
        /* since the this function can increase exponentially
         * used % M to bound within the range */
        result = (result + (key[i] * p_pow) % M) % M;
        // doing the same for p_pow because it increase exponentially
        p_pow = (p_pow * P) % M;
    }

    //returning the value with % size so it doesn't go out of our available index
    return (result % M) % size;
}

/* this function initialize cache with the capacity given as argument */
Cache *init_cache(size_t capacity)
{
    /* Allocating memory for Cache and HashTable */
    Cache *c = malloc(sizeof(Cache));
    c->HashTable = calloc(capacity, sizeof(HashNode *));

    // return NULL if allocation fails
    if (c == NULL || c->HashTable == NULL)
    {
        // this is macro which gives additional information that can be used to debug
        LOG_ERROR_MSG("malloc failed");
        return NULL;
    }

    /* set capacity to given capacity by user
    *  initial size is set to zero
    *  head and tail both point to NULL (empty cache)
    */
    c->capacity = capacity;
    c->size = 0;
    c->head = c->tail = NULL;
    
    // returning the created cache to user
    return c;
}

// Delete all the memory allocated for cache, nodes, hashNodes and keys
void destroy_cache(Cache **c)
{
    // return if cache is not initialized
    if (*c == NULL) return;

    // using delete to store the head pointer so can traverse and delete all the nodes 
    Node *delete = (*c)->head;
    while (delete != NULL)
    {
        // save the next pointer so can avoid using freed memory
        Node *temp = delete->next;
        // this is macro you can find in debug.h file which frees memory and set pointer to NULL
        SAFE_FREE(delete->key);
        SAFE_FREE(delete);
        // after deleting the current node set it to the next one
        delete = temp;
    }

    for (size_t i = 0; i < (*c)->capacity; ++i)
    {
        // following the same strategy we used for nodes
        HashNode *hashNodeDelete = (*c)->HashTable[i];
        while (hashNodeDelete != NULL)
        {
            HashNode *temp = hashNodeDelete->next;
            SAFE_FREE(hashNodeDelete);
            hashNodeDelete = temp;
        }
    }
    /* now after all nodes and hashNodes
    * are successfully deleted its time to 
    * delete HashTable and Cache */
    SAFE_FREE((*c)->HashTable);
    SAFE_FREE(*c);
}

// printing cache with different colors easy for debugging
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

// printing hashtable for debugging
void print_hash_table(Cache *c)
{
    if (c->HashTable == NULL) return;

    for (size_t i = 0; i < c->capacity; ++i)
    {
        HashNode *temp = c->HashTable[i];
        if (temp == NULL) continue;

        printf(YELLOW "(%lu)" RESET " -> ", i);

        // uncomment these lines to see hash table with empty hashNodes
        // if (temp == NULL)
        // {
        //     printf(RED "%p" RESET "\n", temp);
        //     continue;   
        // }

        while (temp != NULL)
        {
            printf("[ " BLUE "%s" RESET " | " GREEN "%d" RESET " ] -> ", 
                   temp->saved_node->key, temp->saved_node->data);
            temp = temp->next;
        }
        printf(RED "%p" RESET "\n", temp);
    }
}

// this is function for searching the existing key in nodes return pointer to node if found
Node *hash_search(Cache *c, const char *key)
{
    // catch the index with hash function, help to find the desired data in O(1) time on average
    uint64_t index = hash(key, c->capacity);
    // storing the pointer to the current hashNode
    HashNode *hash_node = c->HashTable[index];

    while (hash_node != NULL)
    {
        // comparing the provided key to the key of that node which is stored in hashNode
        // if found return to the caller
        if (strcmp(hash_node->saved_node->key, key) == 0) return hash_node->saved_node;
        // move HashNode to the next one
        hash_node = hash_node->next;
    }
    
    // return NULL if not found
    return NULL;
}

/* Be very cautious here we are using the same node used for Cache
* here in hashNode, first node has access to next and prev, but since
* we are using chaining method to deal with collisons, do not use 
* prev, because we only want to go next, can be easy to find when we
* know the only direction to search the data */
void hash_insert (Cache *c, Node *node)
{
    uint64_t index = hash(node->key, c->capacity);

    // check if it already exsit in that case just return to caller
    if (hash_search(c, node->key))
    {
        LOG_ERROR_MSG("key already exist");
        return;
    }

    // create new hash node
    HashNode *hnode = malloc(sizeof(HashNode));
    if (hnode == NULL)
    {
        LOG_ERROR_MSG("malloc failed");
        return;
    }

    // storing the node pointer to our created hash node
    hnode->saved_node = node;

    // inserting new hash node to head easier and faster than finding tail and append
    hnode->next = c->HashTable[index];
    c->HashTable[index] = hnode;
}

// delete hash node function to delete whenever cache remove the node
void hash_delete(Cache *c, const char *key)
{
    uint64_t index = hash(key, c->size);

    // using del to store the hash node we want to delete
    // using prev to keep track of the previous node
    HashNode *del = c->HashTable[index];
    HashNode *prev = NULL;

    // traverse until del reach tail and key is not match
    while (del != NULL && strcmp(del->saved_node->key, key) != 0) {
        prev = del;
        del = del->next;
    } 

    if (del == NULL) return; // key not found so return to caller;
    
    // if prev is NULL that means we want to delete hash node
    if (prev == NULL)
    {
        // in this case we just move head to the next
        c->HashTable[index] = c->HashTable[index]->next;
    }
    // deleting middle or tail
    else prev->next = del->next;

    // delete the node
    SAFE_FREE(del);
}

// Helper function for put() to create node
Node *create_node(const char *key, int data)
{
    // allocate memory for both provided node and key
    Node *node = malloc(sizeof(Node));
    node->key = malloc(strlen(key) + 1);

    if (node == NULL || node->key == NULL)
    {
        LOG_ERROR_MSG("Malloc failed");
        return NULL;
    }
    
    // using strcpy because we already provided 1 extra byte
    // strcpy will add '\0' at the end
    strcpy(node->key, key);
    node->data = data;

    return node;
}

/*
* remove a node from both te cashe and hash table
* updates casche size and handles all cases (head/tail/middle/empty)
*/
void delete_node(Cache *c, Node *del)
{
    // if provided cache or node to delete is invalid return with error message
    if (del == NULL || c == NULL) 
    {
        LOG_ERROR_MSG("invalid node provided");
        return;
    }

    // when there is only one node
    if (del == c->head && del == c->tail)
    {
        c->head = c->tail = NULL;
    }
    // when deleting head
    else if (del == c->head)
    {
        c->head = c->head->next;
        c->head->prev = NULL;
    }
    // when deleting tail
    else if (del == c->tail)
    {
        c->tail = c->tail->prev;
        c->tail->next = NULL;
    }
    // deleting middle node
    else 
    {
        del->prev->next = del->next;
        del->next->prev = del->prev;
    }

    // delete the hash node
    hash_delete(c, del->key);
    // free both the key and node
    SAFE_FREE(del->key);
    SAFE_FREE(del);
    // update size
    --c->size;
}

// helper function for put() to insert head
void insert_head(Cache *c, Node *node)
{
    // since we are inserting head, pointing the prev to NULL
    node->prev = NULL;
    // and it's next to head
    node->next = c->head;

    // if cache is not empty
    if (c->head != NULL)
    {
        // set head->prev to node
        c->head->prev = node;
    }
    else 
    {
        // if empty that means c->tail should point to node
        c->tail = node;
    }
    // just set the head to our new head
    c->head = node;
}

/*
    * this is helper function for both put() and get()
    * to move the most recently used cache node to head
*/
void move_to_head(Cache *c, Node *node)
{
    if (node == c->head) return;

    // if node->prev is not NULL then connect previous one to the next 
    if (node->prev) node->prev->next = node->next;
    // same if node->next exist
    if (node->next) node->next->prev = node->prev;
    // update the new tail
    else c->tail = node->prev;

    // use insert_head() to insert head
    insert_head(c, node);
}

// this helper function just remove the tail call it when cache is full
void evict_tail(Cache *c)
{
    delete_node(c, c->tail);
}

/* 
    * this is our put function which put the node to the cache
    * ceated the helper functions so this can be easy and readable
*/
void put(Cache *c, const char *key, int data)
{
    // return with error message if cache doesn't exist
    if (c == NULL) 
    {
        LOG_ERROR_MSG("Cache doesn't exist");
        return;
    }

    // check if node exist or not
    Node *exist = hash_search(c, key);

    if (exist)
    {
        // if node exist just update data and move node to the head
        exist->data = data;
        move_to_head(c, exist);
        return;
    }

    // if cache is full use evict_tail() to remove tail
    if (c->size == c->capacity) 
    {
        evict_tail(c);
    }

    // after all cases are dealt with create new node and insert to head
    Node *node = create_node(key, data);
    insert_head(c, node);
    // don't forget to insert that in hash table
    hash_insert(c, node);               
    // update the size
    ++c->size;
}

/* 
* this function just does two things
* if it finds the key returns the data and move node to head
* if key doesn't exist returns -1
*/
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


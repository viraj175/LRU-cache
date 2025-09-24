#include "lcache.h"
#include "debug.h"

#include <stdio.h>

#define CAPACITY 40

int main() {
    printf("Welcome to LRUcache project\n");

    Cache *c1 = init_cache(CAPACITY);
    put(c1, "int", 34);
    print_cache(c1);
    put(c1, "INT", 4);
    print_cache(c1);
    put(c1, "viraj", 3);
    print_cache(c1);
    put(c1, "viraj", 23);
    print_cache(c1);
    print_hash_table(c1);
    put(c1, "shield", 48);
    print_cache(c1);
    put(c1, "hint", 32);
    print_cache(c1);
    CALL_LOG_GET(get(c1, "int"));
    print_cache(c1);
    CALL_LOG_GET(get(c1, "hi"));
    print_cache(c1);
    print_hash_table(c1);

    destroy_cache(&c1);
    
    return 0;
}

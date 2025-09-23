#include "lcache.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int main() {
    printf("Welcome to LRUcache project\n");

    Cache *c1 = init_cache(5);
    put(c1, "hello", 19);
    put(c1, "viraj", 10);
    put(c1, "hint", 12);
    put(c1, "int", 34);
    put(c1, "INT", 4);
    print_cache(c1);
    put(c1, "viraj", 3);
    print_cache(c1);
    put(c1, "shield", 48);
    print_cache(c1);
    put(c1, "hint", 32);
    print_cache(c1);

    destroy_cache(&c1);
    
    return 0;
}

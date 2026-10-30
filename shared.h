#ifndef SHARED_H
    #define ARRAY_LEN 10
    #define SHARED_H
    #define Fn(F, X) void (*F)(Item, X)

    typedef int Item;
    typedef void (*Fn)(Item);

    #include <stdlib.h>
    #include <stdbool.h>
#endif

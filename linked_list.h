#include "shared.h"

typedef struct llnode {
    Item item;
    struct llnode *next;
} LinkedListNode;

typedef struct {
    LinkedListNode *head;
    LinkedListNode *tail;
    int size;
} LinkedList;

LinkedList new_linked_list(void);

bool add_item(const Item *item, LinkedList *list);

bool insert_item(const Item *item, int index, LinkedList *list);

// bool remove_item(const Item *item, LinkedList *list);

void traverse(LinkedList *list, void (*fun)(Item));

int len(const LinkedList *list);

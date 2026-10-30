#include "shared.h"
#include "linked_list.h"

LinkedList new_linked_list(void) {
    return (LinkedList) {
        .head = NULL,
        .tail = NULL,
        .size = 0
    };
}

static bool create_node_and_attach(const Item *item, LinkedListNode **ppn) {
    *ppn = malloc(sizeof(LinkedListNode));

    if (*ppn != NULL) {
        (*ppn)->item = *item;
        (*ppn)->next = NULL;
        return true;
    }

    return false;
}

bool add_item(const Item *item, LinkedList *list) {
    if (list->head == NULL) {
        bool res = create_node_and_attach(item, &list->head);
        
        if (res) {
            list->tail = list->head;
            list->size++;
        }

        return res;
    }

    bool res = create_node_and_attach(item, &list->tail->next);
    
    if (res) {
        list->tail = list->tail->next;
        list->size++;
    }

    return res;
}

bool insert_item(const Item *item, int index, LinkedList *list) {
    if (index >= list->size) {
        return false;
    }

    if (index == 0) {
        LinkedListNode *head = list->head;
        bool res = create_node_and_attach(item, &list->head);

        if (res) {
            list->head->next = head;
            list->size++;
        }

        return res;
    }

    if (index == list->size - 1) {
        return add_item(item, list);
    }

    LinkedListNode *parent = list->head;

    for (int i = 0; i < index; i++) {
        parent = parent->next;
    }

    LinkedListNode *child = parent->next;

    bool res = create_node_and_attach(item, &parent->next);

    if (res) {
        parent->next->next = child;
        list->size++;
    }

    return res;
}

void traverse(LinkedList *list, void (*fun)(Item)) {
    LinkedListNode *node = list->head;

    while (node != NULL) {
        fun(node->item);
        node = node->next;
    }
}

int len(const LinkedList *list) {
    return list->size;
}

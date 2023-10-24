#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__
#include "util.h"

typedef struct __node
{
    TYPE type;
    int length;
    char *content;
    struct __node *next;
    struct __node *prev;
} Node;

typedef struct _list
{
    int num;
    Node *head;
    Node *tail;
} LinkedList;

Node *create_node(TYPE type, char *content);

void insert_at_head(LinkedList *list, Node *node);

void insert_at_tail(LinkedList *list, Node *node);

void insert_at_next(LinkedList *list, Node *node, Node *toInsert);

void insert_at_before(LinkedList *list, Node *node, Node *toInsert);

void delete_node(LinkedList *list, Node *Node);

void print_list(LinkedList *list);

void delete_all_node(LinkedList *list);

#endif
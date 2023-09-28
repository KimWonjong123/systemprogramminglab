#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct _node
{
    int lineNum;
    int length;
    char *content;
    struct _node *next;
    struct _node *prev;
} Node;

typedef struct list
{
    int num;
    Node *head;
    Node *tail;
} LinkedList;

Node *create_node(int lineNum, char *content);

void insert_at_head(LinkedList *list, Node *node);

void insert_at_tail(LinkedList *list, Node *node);

void insert_at_next(LinkedList *list, Node *node, Node *toInsert);

void insert_at_before(LinkedList *list, Node *node, Node *toInsert);

void delete_node(LinkedList *list, Node *Node);

void print_list(LinkedList *list);

void delete_all_node(LinkedList *list);

#endif
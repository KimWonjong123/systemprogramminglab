#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

typedef struct _node
{
    int lineNum;
    long long offset;
    long long size;
    struct _node *next;
    struct _node *prev;
} Node;

typedef struct list
{
    int num;
    Node *head;
    Node *tail;
} LinkedList;

Node *create_node_idx(long long offset, long long size, int lineNum);

void insert_at_head_idx(LinkedList *list, Node *node);

void insert_at_tail_idx(LinkedList *list, Node *node);

void insert_at_next_idx(LinkedList *list, Node *node, Node *toInsert);

void insert_at_before_idx(LinkedList *list, Node *node, Node *toInsert);

void delete_node_idx(LinkedList *list, Node *Node);

// void print_list_idx(LinkedList *list);

void delete_all_node_idx(LinkedList *list);

#endif
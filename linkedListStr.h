#ifndef __LINKEDLISTSTR_H__
#define __LINKEDLISTSTR_H__

typedef struct _node
{
    int lineNum;
    int length;
    char *content;
    struct _node *next;
    struct _node *prev;
} NodeStr;

typedef struct list
{
    int num;
    NodeStr *head;
    NodeStr *tail;
} LinkedListStr;

NodeStr *create_node(int lineNum, char *content);

void insert_at_head(LinkedListStr *list, NodeStr *node);

void insert_at_tail(LinkedListStr *list, NodeStr *node);

void insert_at_next(LinkedListStr *list, NodeStr *node, NodeStr *toInsert);

void insert_at_before(LinkedListStr *list, NodeStr *node, NodeStr *toInsert);

void delete_node(LinkedListStr *list, NodeStr *Node);

void print_list(LinkedListStr *list);

void delete_all_node(LinkedListStr *list);

#endif
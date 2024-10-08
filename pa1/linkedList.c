#include "linkedList.h"
#include <unistd.h>
#include <stdlib.h>

Node *create_node_idx(long long offset, long long size, int lineNum)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->offset = offset;
    node->size = size;
    node->lineNum = lineNum;
    node->next = NULL;
    node->prev = NULL;
    return node;
};

void insert_at_head_idx(LinkedList *list, Node *node)
{
    if (list->head == NULL)
    {
        list->head = list->tail = node;
    }
    else
    {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->num++;
}

void insert_at_tail_idx(LinkedList *list, Node *node)
{
    if (list->tail == NULL)
    {
        list->head = list->tail = node;
    }
    else
    {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }
    list->num++;
}

void insert_at_next_idx(LinkedList *list, Node *node, Node *toInsert)
{
    if (node->next == NULL)
    {
        insert_at_tail_idx(list, node);
        return;
    }
    toInsert->prev = node;
    toInsert->next = node->next;
    node->next = node->next->prev = toInsert;
    list->num++;
}

void insert_at_before_idx(LinkedList *list, Node *node, Node *toInsert)
{
    if (node->prev == NULL)
    {
        insert_at_head_idx(list, node);
        return;
    }
    toInsert->next = node;
    toInsert->prev = node->prev;
    node->prev = node->prev->next = toInsert;
    list->num++;
}

void delete_node_idx(LinkedList *list, Node *node)
{
    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    else {
        list->head = node->next;
    }
    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else {
        list->tail = node->prev;
    }
    list->num--;
    free(node);
}

// void print_list(LinkedList *list)
// {
//     Node *node = list->head;
//     while (node != NULL)
//     {
//         write(1, (char *)node->content, node->length);
//         write(1, "\n", 1);
//         node = node->next;
//     }
// }

void delete_all_node_idx(LinkedList *list)
{
    Node *node = list->head;
    Node *temp = list->head;
    while (node != NULL)
    {
        node = node->next;
        free(temp);
        temp = node;
    }
    list->num = 0;
    list->head = list->tail = NULL;
}

#include "linkedListStr.h"
#include <unistd.h>
#include <stdlib.h>
#include "mystring.h"

NodeStr *create_node(int lineNum, char *content)
{
    NodeStr *node = (NodeStr *)malloc(sizeof(NodeStr));
    node->lineNum = lineNum;
    node->content = (char *)malloc(stringlen(content) + 1);
    stringcpy(content, node->content);
    node->length = stringlen(node->content);
    node->next = NULL;
    node->prev = NULL;
    return node;
};

void insert_at_head(LinkedListStr *list, NodeStr *node)
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

void insert_at_tail(LinkedListStr *list, NodeStr *node)
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

void insert_at_next(LinkedListStr *list, NodeStr *node, NodeStr *toInsert)
{
    if (node->next == NULL)
    {
        insert_at_tail(list, node);
        return;
    }
    toInsert->prev = node;
    toInsert->next = node->next;
    node->next = node->next->prev = toInsert;
    list->num++;
}

void insert_at_before(LinkedListStr *list, NodeStr *node, NodeStr *toInsert)
{
    if (node->prev == NULL)
    {
        insert_at_head(list, node);
        return;
    }
    toInsert->next = node;
    toInsert->prev = node->prev;
    node->prev = node->prev->next = toInsert;
    list->num++;
}

void delete_node(LinkedListStr *list, NodeStr *node)
{
    free(node->content);
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

void print_list(LinkedListStr *list)
{
    NodeStr *node = list->head;
    while (node != NULL)
    {
        write(1, (char *)node->content, node->length);
        write(1, "\n", 1);
        node = node->next;
    }
}

void delete_all_node(LinkedListStr *list)
{
    NodeStr *node = list->head;
    while (node != NULL)
    {
        delete_node(list, node);
        node = list->head;
    }
}

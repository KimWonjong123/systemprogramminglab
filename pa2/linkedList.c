#include "linkedList.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

Node *create_node(TYPE type, char *content)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->type = type;
    node->content = (char *)malloc(strlen(content) + 1);
    strcpy(node->content, content);
    node->length = strlen(node->content);
    node->next = NULL;
    node->prev = NULL;
    return node;
};

void insert_at_head(LinkedList *list, Node *node)
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

void insert_at_tail(LinkedList *list, Node *node)
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

void insert_at_next(LinkedList *list, Node *node, Node *toInsert)
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

void insert_at_before(LinkedList *list, Node *node, Node *toInsert)
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

void delete_node(LinkedList *list, Node *node)
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

void print_list(LinkedList *list)
{
    Node *node = list->head;
    while (node != NULL)
    {
        write(1, (char *)node->content, node->length);
        write(1, "\n", 1);
        node = node->next;
    }
}

void delete_all_node(LinkedList *list)
{
    Node *node = list->head;
    while (node != NULL)
    {
        delete_node(list, node);
        node = list->head;
    }
}

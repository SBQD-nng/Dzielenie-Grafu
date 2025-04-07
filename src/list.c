#include "list.h"
#include <stdlib.h>


List* list_new()
{
	List* list = malloc(sizeof(List));
	list->first = NULL;
	list->last = NULL;
	return list;
}

ListNode* list_append(List* list, void* val)
{
	return list_insert(list, list->last, val);
}

ListNode* list_insert(List* list, ListNode* prev, void* val)
{
	ListNode* node = malloc(sizeof(ListNode));
	ListNode* oldFirst = list->first;

	if (prev == NULL)
	{
		list->first = node;
		if (list->last == NULL) list->last = node;
	}
	else if (list->last == prev)
	{
		list->last = node;
	}

	node->val = val;
	node->prev = prev;

	if (prev != NULL)
	{
		if (prev->next != NULL) ((ListNode*)prev->next)->prev = node;
		node->next = prev->next;
		prev->next = node;
	}
	else
	{
		node->next = oldFirst;
	}

	return node;
}

void list_remove(List* list, ListNode* node)
{
	if (node->prev != NULL) ((ListNode*)node->prev)->next = node->next;
	if (node->next != NULL) ((ListNode*)node->next)->prev = node->prev;

	if (list->first == node) list->first = node->next;
	if (list->last == node) list->last = node->prev;

	free(node);
}

#include "list.h"
#include <stdlib.h>


List* list_new()
{
	List* list = malloc(sizeof(List));
	list->first = NULL;
	list->last = NULL;
	list->size = 0;
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

	list->size++;
	return node;
}

List* list_merge(List* list1, List* list2)
{
	// if first is empty
	if (list1->first == NULL)
	{
		free(list1);
		return list2;
	}

	// if second is empty
	if (list2->first == NULL)
	{
		free(list2);
		return list1;
	}

	list1->last->next = list2->first;
	list2->first->prev = list1->last;

	list1->last = list2->last;
	list1->size += list2->size;
	free(list2);

	return list1;
}

void list_remove(List* list, ListNode* node)
{
	if (node->prev != NULL) ((ListNode*)node->prev)->next = node->next;
	if (node->next != NULL) ((ListNode*)node->next)->prev = node->prev;

	if (list->first == node) list->first = node->next;
	if (list->last == node) list->last = node->prev;

	free(node);
	list->size--;
}

void list_free(List* list)
{
	ListNode* node = list->first;
	while (node != NULL)
	{
		ListNode* next = node->next;
		free(node);
		node = next;
	}

	free(list);
}

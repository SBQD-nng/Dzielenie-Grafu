#ifndef _LIST_H_
#define _LIST_H_

typedef struct
{
	void* val;
	void* prev;
	void* next;
} ListNode;

typedef struct
{
	ListNode* first;
	ListNode* last;
} List;


// creates new list
extern List* list_new();

// appends value to the end of list
extern ListNode* list_append(List* list, void* val);

// appends value after list node
extern ListNode* list_insert(List* list, ListNode* prev, void* val);

// removes node from list and from memory (it doesn't remove val)
extern void list_remove(List* list, ListNode* node);

#endif

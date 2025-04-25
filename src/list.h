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
	int size;
} List;


// creates new list
extern List* list_new();

// appends value to the end of list
extern ListNode* list_append(List* list, void* val);

// appends value after list node
extern ListNode* list_insert(List* list, ListNode* prev, void* val);

// merges two lists into one by appending list2 at the end of list1
// returns merged list; list1 and list2 should be treaded as removed lists
List* list_merge(List* list1, List* list2);

// removes node from list and from memory (it doesn't remove val)
extern void list_remove(List* list, ListNode* node);

// removes list and all of its nodes
extern void list_free(List* list);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#ifndef LIST_H
#define LIST_H

struct node
{
	int data;
	struct node *next;
	int rc; // reference counting
};

struct linked_list
{
	int length;
	struct node *first;
	pthread_mutex_t lock;
};

static inline struct node *new_node(int val)
{
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->data = val;
	n->next = NULL;
	n->rc = 0;
	return n;
}

/*
ll_create allocates and returns a pointer to a linked list. If unable to allocate the list, ll_create returns
NULL. You must always check if malloc returns NULL.
*/
static inline struct linked_list *
ll_create(void)
{
	struct linked_list *ll = (struct linked_list *)malloc(sizeof(struct linked_list));
	ll->length = 0;
	ll->first = NULL;
	pthread_mutex_init(&ll->lock, NULL);
	return ll;
}

/*
ll_destroy deallocates a linked list, only if it is empty. Return 1 if the linked list is destroyed, and 0 if it
couldn’t be destroyed.
*/
static inline int
ll_destroy(struct linked_list **ll)
{
	pthread_mutex_lock(&(*ll)->lock);
	if ((*ll)->length == 0)
	{
		free(*ll);
		pthread_mutex_unlock(&(*ll)->lock);
		*ll = NULL;
		return 1;
	}
	else
		pthread_mutex_lock(&(*ll)->lock);

	return 0;
}

/*
ll_add inserts a value at the head of the linked list.
*/
static inline void
ll_add(struct linked_list *ll, int value)
{
    struct node* new_node = (struct node*)malloc(sizeof(struct node));
    new_node->data = value;

    struct node* expected;
    do {
        expected = ll->first;
        new_node->next = expected;
    } while (!__sync_bool_compare_and_swap(&ll->first, expected, new_node));

}

/*
ll_length returns the total number of values in the linked list. -1 if ll DNE.
*/
static inline int
ll_length(struct linked_list *ll)
{
	// If ll_remove_first is entered first but ll_destroy finishes in the meantime.
	if (ll == NULL)
		return -1;
	return ll->length;
}

/*
ll_remove_first removes the value at the head of the linked list and returns true. If the list is empty,
ll_remove_first returns false.
*/
static inline bool
ll_remove_first(struct linked_list *ll)
{
    struct node* head;
    struct node* next;

    do {
        head = ll->first;
        if (head == NULL) {
            return false; // List is empty.
        }
        next = head->next;
    } while (!__sync_bool_compare_and_swap(&ll->first, head, next));

    free(head);
    return true;
}
/*
ll_contains searches the linked list from head to tail and returns the first position at which the value
is found. In a list with n values, the head is position 1 and the tail is position n; therefore, if the value
is in the list, ll_contains returns a logical true, the offset into the linked list. If the value is not found in
the list, 0 is returned; therefore, if the value is not in the list, ll_contains returns a logical false. Most
often, this will be used to determine if a node is in the linked list, and not where it is in the list, thus
the optimization to have it return 0 (logical false), and a 1-indexed offset otherwise (logical true).
*/

static inline int
ll_contains(struct linked_list *ll, int value)
{
	struct node *iter = ll->first;
	int count;
	count = 0;

	while (iter)
	{
        __sync_fetch_and_add(&iter->rc, 1);
		count += 1;

		if (iter->data == value)
		{
			return count;
		}
        __sync_fetch_and_sub(&iter->rc, 1);
		iter = iter->next;
	}

	return count;
}

#endif

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_THREADS 20000

typedef struct
{
	struct linked_list *list;
	int thread_id;
} thread_arg;

struct removal_counter
{
	int empty_remove_cnt;
	pthread_mutex_t mutex;
};

struct removal_counter test_interleave_remove_counter = {.empty_remove_cnt = 0, .mutex = PTHREAD_MUTEX_INITIALIZER};

void *
concurrent_add(void *arg)
{
	thread_arg *args = (thread_arg *)arg;
	ll_add(args->list, args->thread_id);
	pthread_exit(NULL);
}

void *concurrent_remove(void *arg)
{
	thread_arg *args = (thread_arg *)arg;
	if (!ll_remove_first(args->list))
	{
		pthread_mutex_lock(&test_interleave_remove_counter.mutex);
		test_interleave_remove_counter.empty_remove_cnt += 1;
		pthread_mutex_unlock(&test_interleave_remove_counter.mutex);
	}
	pthread_exit(NULL);
}
void test_mass_add()
{
	// Objective: No adds should be skipped
	// Test this by ensuring ll->length is 100000.

	pthread_t threads[NUM_THREADS];
	int t;
	struct linked_list *list = ll_create();
	for (t = 0; t < NUM_THREADS; t++)
	{
		thread_arg args;
		args.list = list;
		args.thread_id = t;
		pthread_create(&threads[t], NULL, concurrent_add, (void *)&args);
	}

	for (t = 0; t < NUM_THREADS; t++)
	{
		pthread_join(threads[t], NULL);
	}

	assert(list->length == NUM_THREADS);
}

void test_mass_remove()
{
	// Objective: No removes should be skipped
	// Test this by removing NUM_THREADS nodes from a list of NUM_THREADS elements
	// Should end with an empty list.

	pthread_t threads[NUM_THREADS];
	struct linked_list *list = ll_create();
	int t;
	/* BEGIN: Add some stuff to the linked list, might as well do it concurrently */
	for (t = 0; t < NUM_THREADS; t++)
	{
		thread_arg args;
		args.list = list;
		args.thread_id = t;
		pthread_create(&threads[t], NULL, concurrent_add, (void *)&args);
	}
	for (t = 0; t < NUM_THREADS; t++)
	{
		pthread_join(threads[t], NULL);
	}
	/* END: Add some stuff to the linked list */

	/* BEGIN: Removing from linked list */
	for (t = 0; t < NUM_THREADS; t++)
	{
		thread_arg args;
		args.list = list;
		args.thread_id = t;
		pthread_create(&threads[t], NULL, concurrent_remove, (void *)&args);
	}
	for (t = 0; t < NUM_THREADS; t++)
	{
		pthread_join(threads[t], NULL);
	}
	/* END: Removing from linked list */

	assert(list->length == 0);
}

void test_add_remove_interleaved(int thread_cnt)
{
	/* Objective: Make sure that an interleaving of add and remove does not skip any operations
	   How to do this? Well, perform X concurrent_adds and Y concurrent_removes.
	   The resultant list length should be "X - Y + empty_removes", with an lower bound of 0.
	   The "X-Y" part should be obvious, the empty_removes requires more scrutiny:
	   Removes can happen before adds, such removes doesn't impact the final count but are implicitly counted by Y,
	   so, we offset them correctly by "+ empty_removes".
	*/

	pthread_t threads[thread_cnt];

	struct linked_list *list = ll_create();

	int add_cnt = 0, remove_cnt = 0;

	srand(time(NULL)); // randomize seed everytime, or else, rand() runs with seed 1

	for (int t = 0; t < thread_cnt; t++)
	{
		thread_arg args;
		args.list = list;
		args.thread_id = t;

		if (rand() % 2 == 0)
		{ // 50% add
			add_cnt += 1;
			pthread_create(&threads[t], NULL, concurrent_add, (void *)&args);
		}
		else
		{ // 50% remove
			remove_cnt += 1;
			pthread_create(&threads[t], NULL, concurrent_remove, (void *)&args);
		}
	}

	for (int t = 0; t < thread_cnt; t++)
	{
		pthread_join(threads[t], NULL);
	}

	printf("Add Counts: %d, Remove Count: %d, Empty removes: %d, Linked List Length: %d\n", add_cnt, remove_cnt, test_interleave_remove_counter.empty_remove_cnt, list->length);

	int res = add_cnt - remove_cnt + test_interleave_remove_counter.empty_remove_cnt;
	res = res < 0 ? 0 : res;
	printf("%d\n", res);
	assert(list->length == res);

	// reset empty remove cnt for future test_add_remove_interleaved
	test_interleave_remove_counter.empty_remove_cnt = 0;
}

void print_ll(struct linked_list *ll)
{
	struct node *iter = ll->first;
	printf("ll contents: ");
	while (iter != NULL)
	{
		if (iter->next)
		{
			printf("%d, ", iter->data);
		}
		else
		{
			printf("%d\n", iter->data);
		}
		iter = iter->next;
	}
}

int main(void)
{
	// printf("Starting test suite...\n");

	// printf("Running Test 1: Mass add\n");
	// test_mass_add();
	// printf("Passed Test 1\n\n");

	// printf("Running Test 2: Mass remove\n");
	// test_mass_remove();
	// printf("Passed Test 2\n\n");

	printf("Running Test 3: Interleaved mass add and remove\n");
	test_add_remove_interleaved(NUM_THREADS);
	printf("Passed Test 3\n\n");

	printf("Running Test 4: Multiple, smaller interleaved mass add and remove\n");
	for (int i = 0; i < 100; i++)
		test_add_remove_interleaved(100);
	printf("Passed Test 4\n");
}

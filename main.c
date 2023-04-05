#include <assert.h>
#include <string.h>
#include "list.h"

void print_ll(struct linked_list* ll){
	struct node* iter = ll->first;
	printf("ll contents: ");
	while(iter != NULL){
		if (iter->next){
			printf("%d, ", iter->data);
		}else{
			printf("%d\n", iter->data);
		}
		iter = iter->next;
	}
}

int
main(void)
{
	struct linked_list* ll = ll_create();
	assert(ll_contains(ll, 1) == 0);
	assert(ll_length(ll) == 0);

	ll_add(ll, 1);
	assert(ll_length(ll) == 1);
	assert(ll_contains(ll, 1) == 1);

	ll_add(ll, 2);
	assert(ll_contains(ll, 1) == 2);

	ll_add(ll, 3);
	assert(ll_contains(ll, 1) == 3);

	// 3, 2, 1
	print_ll(ll);

	ll_remove_first(ll);
	// 2, 1
	print_ll(ll);

	ll_add(ll, 4);
	// 4, 2, 1
	print_ll(ll);

	ll_remove_first(ll);
	// 2, 1
	print_ll(ll);

	ll_add(ll, 5);
	// 5, 2, 1
	print_ll(ll);

	ll_add(ll, 6);
	// 6, 5, 2, 1
	print_ll(ll);

	// ll should be not destroyed
	assert(ll_destroy(ll) == 0);

	assert(ll_contains(ll, 6) == 1);
	assert(ll_contains(ll, 5) == 2);
	assert(ll_contains(ll, 2) == 3);
	assert(ll_contains(ll, 1) == 4);

	ll_remove_first(ll);
	ll_remove_first(ll);
	ll_remove_first(ll);
	assert(ll_remove_first(ll) > 0);
	assert(ll_remove_first(ll) == 0);

	assert(ll_destroy(ll) > 0);


	return 0;
}

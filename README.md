# Instructions

Run locked version

```
make clean && make lock && ./test
```

Run lock-free version

```
make clean && make nolock && ./test
```

We admit that there exist bugs in the lock-free linked-list. 

1) We don't have code that solves the problem of `ll_contains` reading a deleted node. We have a `rc` (reference count) in our `Node` but it's not implemented/utilized.
2) Tests 5 and 6 check for `ll_contains` compatibility with `ll_remove_first` by comparing the index `ll_contains` returns with the initial index in the linked list, if a single index is greater than its initial index. Neither of these tests pass currently.


Willem and I really enjoyed the assignment. Though we didn't complete it, we learned a lot, and we do plan to tackling this in our free time.
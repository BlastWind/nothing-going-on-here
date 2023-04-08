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
1) `ll_remove_first` runs into a double free error via Test 4. This only gets caught by our test suite roughly 1 out of 100 times. We ran the test suite hundreds of times to catch these obscurities.
`for i in {1..100}; do ./test; done;`
2) We don't have code that solves the problem of `ll_contains` reading a deleted node. We have a `rc` (reference count) in our `Node` but it's not implemented/utilized.


Willem and I really enjoyed the assignment. Though we didn't complete it, we learned a lot, and we do plan to tackling this in our free time.
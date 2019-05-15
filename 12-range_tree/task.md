You are given an implementation of a splay tree which associates every numeric
key with a numeric value. The splay tree provides `lookup`, `insert`, and `remove`
operations.

Your goal is to modify the splay tree to support range queries in amortized
logarithmic time. The operation you need to implement takes an range on the
input and should return the sum of values of the elements in the given range.

As usual, you should submit only the `range_tree.{h,py}` file.

## Optional: Range updates (for 5 points)

If you also implement an operation
```
range_update(left, right, delta)
```
which adds `delta` to the value of all elements with key in `[left, right]` range
and runs in amortized logarithmic time, you will get 5 points.

Currently there are no automated tests for this method; therefore, if you
implement it, submit the solution to ReCodEx and write an email to your
teaching assistant.

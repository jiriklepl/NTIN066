class Node:
    """Node of `FibonacciHeap`.

    Property `payload` can be used to store any information
    the user needs.
    """

    def __init__(self, prio, payload = None):
        self.payload = payload

        self._prio = prio
        self._parent = None

        self._rank = 0
        self._first_child = None

        # Pointers in a cyclic list of children of the parent.
        self._prev_sibling = self
        self._next_sibling = self
        self._marked = False

    def priority(self):
        """Return priority of the node."""
        return self._prio

class FibonacciHeap:
    """Fibonacci heap.

    Implemented using a meta root to avoid special handling of the
    linked list of heap trees.
    """
    def __init__(self):
        self._size = 0
        self._meta_root = Node(None)

    def is_empty(self):
        """Check whether the heap is empty."""
        return self._size == 0

    def insert(self, prio, payload = None):
        """Insert a new node with given priority and payload and return it.

        `payload` can be used to store any data in the node.
        """
        n = Node(prio, payload)
        self._add_child(self._meta_root, n)
        self._size += 1
        return n

    def extract_min(self):
        """Extract node with minimum priority.

        Must not be called when the heap is empty.
        Returns tuple (payload, priority) of the removed node.
        """
        assert self._size > 0, "Cannot extract minimum of an empty heap."

        # Find the tree whose root is minimal.
        minimum = self._meta_root._first_child
        node = self._meta_root._first_child._next_sibling
        while node is not self._meta_root._first_child:
            if node._prio < minimum._prio:
                minimum = node
            node = node._next_sibling

        self._remove(minimum)
        self._size -= 1

        # Add all its subtrees to the heap.
        while minimum._rank > 0:
            self._add_child(self._meta_root, self._remove(minimum._first_child))

        # Finally, consolidate the heap.
        self._consolidate()

        return (minimum.payload, minimum._prio)

    def _consolidate(self):
        """INTERNAL: Consolidate heap during extract_min.
        """
        buckets = [ None ] * self._max_rank()
        while self._meta_root._rank > 0:
            node = self._remove(self._meta_root._first_child)
            while buckets[node._rank] is not None:
                b = node._rank
                node = self._pair_trees(node, buckets[b])
                buckets[b] = None
            buckets[node._rank] = node

        for node in buckets:
            if node is not None:
                self._add_child(self._meta_root, node)

    def _pair_trees(self, a, b):
        """INTERNAL: Join two trees of the same rank.
        """
        if a._prio > b._prio: a, b = b, a
        self._add_child(a, b)
        return a

    def _max_rank(self):
        """INTERNAL: Return maximum possible rank of a tree in a heap of the current size.
        """
        ret = 1
        while (1 << ret) <= self._size: ret += 1
        return ret

    def decrease(self, node, new_prio):
        """Decrease priority of node to new_prio.

        new_prio must not be higher than node._prio.
        """
        assert node is not None, "Cannot decrease None."
        assert node is not self._meta_root, "Cannot decrease meta root."
        assert new_prio <= node._prio, "Decrease: new priority is bigger than old one."
        # TODO: Implement
        raise NotImplementedError

    def _is_root(self, node):
        """Check whether node is root."""
        return node._parent is self._meta_root

    def _add_child(self, parent, node):
        """INTERNAL: Add node as child of parent.
        """
        assert parent is not None, "_add_child: Parent cannot be None."
        assert node is not None, "_add_child: Cannot add None as a child."
        assert node._parent is None, "_add_child: node already has a parent."
        assert parent is self._meta_root or node._rank == parent._rank, \
            "_add_child: ranks of node and parent are different"
        assert node._prev_sibling is node and node._next_sibling is node, \
            "_add_child: node has a sibling"
        assert parent is self._meta_root or parent._prio <= node._prio, \
            "_add_child: parent has bigger priority than node"

        if parent._rank == 0:
            parent._first_child = node
        else:
            self._join(parent._first_child._prev_sibling, node)
            self._join(node, parent._first_child)

        node._parent = parent
        parent._rank += 1

    def _remove(self, node):
        """INTERNAL: Disconnect node from parent.
        """
        assert node._parent is not None, "_remove: Cannot disconnect node without parent."
        assert node is not self._meta_root, "_remove: Cannot remove meta root."

        node._parent._rank -= 1
        if node._parent._rank == 0:
            node._parent._first_child = None
        elif node._parent._first_child is node:
            node._parent._first_child = node._next_sibling

        node._parent = None
        self._join(node._prev_sibling, node._next_sibling)
        self._join(node, node)
        return node

    def _join(self, a, b):
        """INTERNAL: Link together two elements of linked list -- a and b.
        """
        a._next_sibling = b
        b._prev_sibling = a

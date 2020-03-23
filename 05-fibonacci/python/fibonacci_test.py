#!/usr/bin/env python3

from fibonacci import FibonacciHeap as Heap
from math import log

def children(root, recursive = True):
    """Iterate over all children of `root`.
    """
    if root._first_child is None: return

    node = root._first_child
    while True:
        yield node
        if recursive and node._rank > 0:
            node = node._first_child
        else:
            while node._next_sibling is node._parent._first_child:
                node = node._parent
                if node is root: return
            node = node._next_sibling

def show_node(node, indent):
    """Helper of show_heap.
    """
    print("%s- %s (payload: %s)" % ("  " * indent, node.priority(), node.payload))
    for child in children(node, False):
        show_node(child, indent + 1)

def show_heap(heap):
    """Show heap in human-readable form.
    """
    print("Heap of size %i" % heap._size)
    for root in children(heap._meta_root, False):
        show_node(root, 0)
    print("")

def audit_heap(heap):
    """Check various heap invariants.
    """
    assert heap._meta_root is not None, "Meta root is None"
    assert heap._size == sum( 1 for _ in children(heap._meta_root) ), \
        "Size of the heap does not match real number of its nodes."
    assert all( root._marked == False for root in children(heap._meta_root, \
        recursive=False) ), "A root is marked."

    for root in children(heap._meta_root):
        assert heap._is_root(root) or root._parent.priority() <= root.priority(), \
            "Parent has bigger priority than its child."

        child_sizes = []
        for c in children(root, False):
            child_sizes.append(c._rank)
            assert c._parent is root, "Inconsistent parent pointer."
            assert c._next_sibling._prev_sibling is c, "Inconsistent sibling pointers."
            assert c._prev_sibling._next_sibling is c, "Inconsistent sibling pointers."

        assert len(child_sizes) == root._rank, \
            "Rank of node does not match its real number of children."
        for i, s in enumerate(sorted(child_sizes)):
            assert s >= i - 1, "Child of the node has too small rank."
 

def remove(H, node):
    """Remove node from heap H.
    """
    H.decrease(node, -10**9)
    H.extract_min()

def consolidate(H):
    """Force consolidation of heap H.
    """
    remove(H, H.insert(0))

def remove_subtree(H, node):
    """Remove subtree rooted in node from the heap.

    Note that this method slightly cheats because it
    learns what nodes are in the subtree by walking over it.
    """
    to_remove = list(children(node))
    to_remove.append(node)

    for node in to_remove: remove(H, node)

def build_sparse_tree(H, rank, prio):
    """Build a tree of a given rank with as few nodes as possible.

    Return pair (root of the tree, its child with largest rank).
    """
    if rank == 0:
        return (H.insert(prio), None)

    a, _ = build_sparse_tree(H, rank - 1, prio)
    b, to_remove = build_sparse_tree(H, rank - 1, prio + 1)

    consolidate(H)
    if to_remove is not None:
        remove_subtree(H, to_remove)

    return (a, b)

def check_path(H):
    """Check whether H contains exactly one path and nothing else.
    """
    node = H._meta_root
    while node._rank == 1: node = node._first_child
    assert node._rank == 0, "Expected path but found node of rank larger than 1."

def build_path(H, length):
    """Build a path of given length.

    Return lowest node on this path. All nodes on path are marked.
    """
    to_remove = []
    H.insert(length + 1)
    end = H.insert(length + 2)
    consolidate(H)
    to_remove.append(H.insert(length + 2))
    a = H.insert(length + 3)
    consolidate(H)
    remove(H, a)

    for i in range(length, 0, -1):
        H.insert(i)
        to_remove.append(H.insert(i + 1))
        consolidate(H)
        a = H.insert(i + 1)
        b = H.insert(i + 2)
        consolidate(H)
        remove(H, b)
        remove(H, a)

    for n in to_remove:
        remove(H, n)

    check_path(H)

    return end

def test_random(size, do_print):
    """A simple random test.

    It does in order:
    - randomly insert elements,
    - do random decreases,
    - extract_min until the heap is empty.
    """
    H = Heap()
    node_map = []

    for i in range(size):
        prio = (1009 * i) % 2099
        node_map.append([H.insert(prio, payload=i), prio])

    consolidate(H)
    if do_print: show_heap(H)

    for i in range(size):
        if i % (size // 10) == 0:
            audit_heap(H)
        idx = (i * 839) % len(node_map)
        n, prio = node_map[idx]
        assert n.priority() == prio, "Priority of node changed but its was not decreased."
        new_prio = prio - ((i * 131 + 15) % 239)
        H.decrease(n, new_prio)
        assert n.priority() == new_prio, "Decrease failed to change priority of the node."
        node_map[idx][1] = new_prio
        if do_print:
            print("Decrease %s -> %s (payload %s)" % (prio, new_prio, n.payload))
            show_heap(H)

    last = None
    audit_heap(H)
    while not H.is_empty():
        payload, prio = H.extract_min()
        assert last is None or prio >= last, "extract_min is not monotone."
        assert node_map[payload][0], \
            "Extracted node was already deleted or something changed its payload."
        assert prio == node_map[payload][1], "Priority of extracted node is wrong."
        last = prio
        node_map[payload] = None
        if do_print:
            print("Extract min %s (payload %s)" % (prio, payload))
            show_heap(H)

    assert all( x is None for x in node_map ), "Not all nodes were deleted."

def test_sparse_tree(rank):
    """Build a sparse tree of given rank and then empty the heap."""
    H = Heap()
    build_sparse_tree(H, rank, 1)
    audit_heap(H)
    while not H.is_empty():
        H.extract_min()

def test_path(length):
    """Build a path, decrease it lowest node, and empty the heap."""
    H = Heap()
    H.decrease(build_path(H, length), -1000)
    assert all( n._rank == 0 for n in children(H._meta_root, False) ), \
        "Decrease of lowest node should have broken the path into forest " + \
        "of isolated vertices but it did not."
    audit_heap(H)
    while not H.is_empty():
        H.extract_min()

tests = [
    ("small",  lambda: test_random(11, True)),
    ("random", lambda: test_random(10000, False)),
    ("sparse", lambda: test_sparse_tree(16)),
    ("path",   lambda: test_path(5000)),
]

if __name__ == "__main__":
    import sys
    for required_test in sys.argv[1:] or [name for name, _ in tests]:
        for name, test in tests:
            if name == required_test:
                print("Running test {}".format(name), file=sys.stderr)
                test()
                break
        else:
            raise ValueError("Unknown test {}".format(name))

#!/usr/bin/env python3

import sys, itertools, random
from math import sqrt, log, exp
from fibonacci import FibonacciHeap as Heap

class BenchmarkingHeap(Heap):
    """A modified Fibonacci heap for benchmarking.

    We inherit the implementation of operations from the FibonacciHeap class
    and extend it by keeping statistics on the number of operations
    and the total number of structural changes. Also, if naive is turned on,
    the decrease does not mark parent which just lost a child.
    """

    def __init__(self, naive=False):
        Heap.__init__(self)
        self.naive = naive
        self._ops = 0
        self._steps = 0
        self._max_size = 0 # used to get upper bound on number of buckets in naive version

    def stats(self):
        return " %.3f" % (self._steps / max(self._ops, 1))

    def insert(self, *args):
        self._ops += 1
        return Heap.insert(self, *args)

    def decrease(self, node, new_prio):
        self._ops += 1
        if self.naive:
            assert node is not None, "Cannot decrease None."
            assert node is not self._meta_root, "Cannot decrease meta root."
            assert new_prio <= node._prio, \
                "Decrease: new priority is bigger than old one."

            node._prio = new_prio
            if self._is_root(node) or node._prio >= node._parent._prio:
                return

            self._add_child(self._meta_root, self._remove(node))
        else:
            Heap.decrease(self, node, new_prio)

    def extract_min(self):
        self._ops += 1
        return Heap.extract_min(self)

    def _add_child(self, *args):
        self._steps += 1
        Heap._add_child(self, *args)

    def _remove(self, *args):
        self._steps += 1
        return Heap._remove(self, *args)

    def _max_rank(self):
        self._max_size = max(self._max_size, self._size)
        if self.naive: return int(sqrt(2 * self._max_size)) + 2
        return Heap._max_rank(self)

def remove(H, node):
    H.decrease(node, -10**9)
    H.extract_min()

def consolidate(H):
    remove(H, H.insert(0))

def log_range(b, e, n):
    assert 0 < b < e
    for i in range(n):
        yield int(b * exp(log(e / b) / (n - 1) * i))

def star(H, node_map, n, r, consolidate_):
    """Construct a star with n nodes and root index r.
    """
    if n == 1:
        assert node_map[r] is None
        node_map[r] = H.insert(r)
        if consolidate_:
            consolidate(H)
    else:
        star(H, node_map, n - 1, r, False)
        star(H, node_map, n - 1, r + n - 1, True)

        for i in range(r + n, r + 2*n - 2):
            remove(H, node_map[i])
            node_map[i] = None

def test_star(naive):
    """Generates a sequence on which non-cascading heaps need lots of time.
    Source: "Replacing Mark Bits with Randomness in Fibonacci Heaps" Jerry Li and John Peebles, MIT
    -> modified so that only a quadratic number of elements are needed in the star construction.
    """
    for i in range(1, 17): # was 26
        start = 3
        N = start + i * (i+1) // 2
        H = BenchmarkingHeap(naive)
        node_map = [None] * N

        for j in range(i, 0, -1):
            star(H, node_map, j, start, False)
            start += j

        for i in range(1 << i):
            H.insert(1)
            H.insert(1)
            H.extract_min()
            H.extract_min()

        print(N, H.stats())

def test_random(ins, dec, rem, naive):
    """A random test.

    The test does 2N insertions first, and then N random operations insert / decrease /
    extract_min each with probability proprotional to its weight ins / dec / rem.
    """
    for N in log_range(50, 80000, 30):
        H = BenchmarkingHeap(naive)
        node_map = []
        s = ins + dec + rem
        ops = [ 0 ] * ((N*ins) // s) + [ 1 ] * ((N*dec) // s) + [ 2 ] * ((N*rem) // s)
        random.shuffle(ops)

        def insert():
            node_map.append(H.insert(random.randrange(5 * N), len(node_map)))

        for _ in range(2*N): insert()

        for op in ops:
            if op == 0: insert()
            elif op == 1:
                node = random.choice(node_map)
                p = node.priority() - random.randrange(N // 5) - 1
                H.decrease(node, p)
            else:
                i, _ = H.extract_min()
                if i + 1 == len(node_map):
                    node_map.pop()
                else:
                    node_map[i] = node_map.pop()
                    node_map[i].payload = i

        print(N, H.stats())

tests = {
    "star": test_star,
    "random": lambda n: test_random(10, 10, 10, n),
    "biased-10": lambda n: test_random(10, 10, 20, n),
    "biased-40": lambda n: test_random(10, 40, 20, n),
    "biased-70": lambda n: test_random(10, 70, 20, n),
    "biased-100": lambda n: test_random(10, 100, 20, n),
}

if len(sys.argv) == 4:
    test, student_id = sys.argv[1], sys.argv[2]
    if sys.argv[3] == "std":
        naive = False
    elif sys.argv[3] == "naive":
        naive = True
    else:
        raise ValueError("Last argument must be either 'std' or 'naive'")
    random.seed(int(student_id))
    if test in tests:
        tests[test](naive)
    else:
        raise ValueError("Unknown test {}".format(test))
else:
    raise ValueError("Usage: {} <test> <student-id> (std|naive)".format(sys.argv[0]))


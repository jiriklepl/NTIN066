#!/usr/bin/env python3
import math
import sys

from matrix_tests import TestMatrix

def generic_test(N, M, B, max_ratio, debug_level):
    m = TestMatrix(N, M, B, debug_level)
    m.fill_matrix()
    m.reset_stats()
    m.transpose()

    print("\t{} misses in {} accesses".format(m.stat_cache_misses, m.stat_accesses))
    if m.stat_accesses:
        mpa = m.stat_cache_misses / m.stat_accesses
        lb = 1 / B
        ratio = mpa / lb
        print("\t{:.6f} misses/access (lower bound is {:.6f} => ratio {:.6f}, need {:.6f})".format(mpa, lb, ratio, max_ratio))
        assert ratio <= max_ratio, "Algorithm did too many I/O operations."

    m.check_result()

def test_main(a, b, limit, num_items):
    tree = ABTree(a, b)

    # Insert keys
    step = int(limit * 1.618)
    key, audit_time = 1, 1
    for i in range(num_items):
        tree.insert(key)
        key = (key + step) % limit

        # Audit the tree occasionally
        if i == audit_time or i + 1 == num_items:
            audit(tree)
            audit_time = int(audit_time * 1.33) + 1

    # Check the content of the tree
    key = 1
    for i in range(limit):
        assert tree.find(key) == (i < num_items), "Tree contains wrong keys"
        key = (key + step) % limit

# A list of all tests
tests = [
    # name                                  N      M     B  max_ratio  debug_level
    ("small2k",     lambda: generic_test(   8,    32,    8,         8,     2 )),
    ("small",       lambda: generic_test(  13,    64,    8,         4,     2 )),
    ("n100b16",     lambda: generic_test( 100,  1024,   16,         3,     1 )),
    ("n1000b16",    lambda: generic_test(1000,  1024,   16,         3,     1 )),
    ("n1000b64",    lambda: generic_test(1000,  8192,   64,         3,     1 )),
    ("n1000b256",   lambda: generic_test(1000, 65536,  256,         5,     1 )),
    ("n1000b4096",  lambda: generic_test(1000, 65536, 4096,        50,     1 )),
]

if __name__ == "__main__":
    for required_test in sys.argv[1:] or [name for name, _ in tests]:
        for name, test in tests:
            if name == required_test:
                print("Running test {}".format(name), file=sys.stderr)
                test()
                break
        else:
            raise ValueError("Unknown test {}".format(name))

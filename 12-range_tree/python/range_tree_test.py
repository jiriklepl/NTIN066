#!/usr/bin/env python3
import sys

from range_tree import Tree

def test_tree(size, ascending):
    sequence = [pow(7, i, size) for i in range(1, size)]
    if ascending: sequence = sorted(sequence)

    tree = Tree()
    for element in sequence:
        tree.insert(element, element)
    return tree

def test_missing(size, ascending):
    tree = test_tree(size, ascending)

    values = 0
    for _ in range(size):
        values += tree.range_sum(-size, 0)
        values += tree.range_sum(size, 2 * size)
    assert values == 0, "Expected no values in an empty range"

def test_suffixes(size, ascending):
    tree = test_tree(size, ascending)

    for left in range(1, size):
        values = tree.range_sum(left, size - 1)
        expected = size * (size - 1) // 2 - left * (left - 1) // 2
        assert values == expected, "Expected {} for range [{}, {}], got {}".format(expected, left, size - 1, values)

def test_updates(size, ascending):
    tree = test_tree(size, ascending)

    for left in range(1, size):
        tree.remove(left)
        tree.insert(left + size - 1, left + size - 1)
        values = tree.range_sum(left + 1, size + left)
        expected = (size + left) * (size + left - 1) // 2 - (left + 1) * left // 2
        assert values == expected, "Expected {} for range [{}, {}], got {}".format(expected, left + 1, size + left, values)

tests = [
    ("random_missing", lambda: test_missing(13, False)),
    ("random_suffixes", lambda: test_suffixes(13, False)),
    ("random_updates", lambda: test_updates(13, False)),

    ("path_missing", lambda: test_missing(13, True)),
    ("path_suffixes", lambda: test_suffixes(13, True)),
    ("path_updates", lambda: test_updates(13, True)),

    ("random_missing_big", lambda: test_missing(19997, False)),
    ("random_suffixes_big", lambda: test_suffixes(19997, False)),
    ("random_updates_big", lambda: test_updates(19997, False)),

    ("path_missing_big", lambda: test_missing(19997, True)),
    ("path_suffixes_big", lambda: test_suffixes(19997, True)),
    ("path_updates_big", lambda: test_updates(19997, True)),
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

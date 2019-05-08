#!/usr/bin/env python3
import gc
import itertools
import sys

from find_duplicates import find_duplicates

class DataGenerator():
    def __init__(self, base, length, suffix):
        self.digits = "0123456789"[:base]
        self.length = length
        self.suffix = suffix

    def generator(self):
        for first in range(len(self.digits)):
            for sequence in itertools.product(self.digits[first], *[self.digits] * (self.length - 1)):
                yield "".join(sequence)
            yield self.digits[-1 - first] + self.suffix

    def __iter__(self):
        return self.generator()

def test_duplicates(base, length, suffix):
    generator = DataGenerator(base, length, suffix)
    results = find_duplicates(generator)
    gc.collect()

    prefixes = [generator.digits[i] for o in range(0, base // 2) for i in [o, -1 - o]]
    correct = [prefix + suffix for prefix in prefixes]
    assert results == correct, "The generates list of duplicates is not correct, got {} and expected {}".format(results, correct)

tests = [
    ("10k", lambda: test_duplicates(10, 4, "101")),
    ("100k", lambda: test_duplicates(10, 5, "1984")),
    ("1M", lambda: test_duplicates(10, 6, "22222")),
    ("10M", lambda: test_duplicates(10, 7, "314159")),
    ("16M", lambda: test_duplicates(8, 8, "7654321")),
]

if __name__ == "__main__":
    try:
        import resource
        resource.setrlimit(resource.RLIMIT_DATA, (64<<20, 64<<20))
    except:
        pass

    for required_test in sys.argv[1:] or [name for name, _ in tests]:
        for name, test in tests:
            if name == required_test:
                print("Running test {}".format(name), file=sys.stderr)
                test()
                break
        else:
            raise ValueError("Unknown test {}".format(name))

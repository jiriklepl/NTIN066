In this assignment, you are given a large file on input. Your goal is to find
duplicated lines and return every duplicated line once, in the order of their
first occurrences in the file.

The challenging part of this assignment is the fact, that your program has to
run in a limited memory, using at most `64MB`, and the input file can be
considerably larger than this memory limit. However, you can rely on the fact
that the number of duplicated lines is considerably smaller (so that all
duplicated lines fit in the memory at the same time).

Instead of handling a real file, you are given a data generator (an `iterator`
in C++ and a `generator` in Python). Note that limiting memory during the
tests works only on Linux (and not on Windows), and of course also in ReCodEx.

You can use full standard library of Python and C++ in this assignment,
including data structure implementations (also, `bytearray` might come handy).
Note that the largest test in Python can run for several minutes.
As usual, you should submit only the `find_duplicates.{h,py}` file.

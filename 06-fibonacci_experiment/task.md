## Goal

The goal of this assignment is to evaluate your implementation of Fibonacci heap
experimentally and to compare it with a "naive" implementation which does not
mark vertices after cutting a child.

You are given a test program (`fibonacci_experiment`) which calls your
implementation from the previous assignment to perform the following
experiments:

- _Star test:_ Specific sequence of operations on which naive implementation
  creates $Θ(\sqrt{n})$ stars of size 1 up to $Θ(\sqrt{n})$.
- _Random test:_ Test of size $n$ first inserts $2n$ elements and then it does
  $n$ operations of which one third are insertions, one third are decreases
  and one third are extract\_mins in random order.
- _Biased tests:_ Like random test but the weights of operations are different.
  First the test inserts $2n$ elements and then it does random $n$ operations.
  Number of operations of each type is proportional to their weight.
  Weight of insert is 10, weight of extract\_min 20 and weight of decrease is
  the number in the name of the test, and implemented possibilities are 10, 40,
  70 and 100.

The program tries each experiment with different values of $n$. In each try,
it prints the average number of structural changes (adding and removing a child
of a node) per one operation (insert, decrease and extract\_min).

You should perform these experiments and write a report, which contains one
plot of the measured data for each test. The plots should be following:

- _Star test:_ One plot with two curves – one for standard and the other one for naive
  version.
- _Random test:_ One plot with two curves – one for standard and the other one for naive
  version.
- _Biased tests:_ One plot with 4 curves – one for each value of bias, all of them tested
  with standard implementation. Note that we do not do biased test naive implementation.

Each plot should show the dependence
of the average number of structural changes on the test size $n$.

The report should discuss the experimental results and try to explain the observed
behavior using theory from the lectures. (If you want, you can carry out further
experiments to gain better understanding of the data structure and include these
in the report. This is strictly optional.)

You should submit a PDF file with the report (and no source code).
You will get 1 temporary point upon submission if the file is syntactically correct;
proper points will be assigned later.

## Test program

The test program is given three arguments:
- The name of the test (`star`, `random`, `biased-10`, `biased-40`, `biased-70`, `biased-100`).
- The random seed: you should use the last 2 digits of your student ID (you can find
  it in the Study Information System – just click on the Personal data icon). Please
  include the random seed in your report.
- The implementation to test (`std` or `naive`).

The output of the program contains one line per experiment, which consists of
the set size and the average number of structural changes.

## Your implementation

Please use your implementation from the previous exercise. If you used C++,
make sure that the `add_child()`,
`remove()` and `max_rank()` methods are declared virtual (they will
be overriden by the test program). Also, if you are performing any structural changes
without using the `add_child()` and `remove()` helper methods, you need to adjust
the `BenchmarkingHeap` class accordingly.

## Hints

The following tools can be useful for producing nice plots:
- [pandas](https://pandas.pydata.org/)
- [matplotlib](https://matplotlib.org/)
- [gnuplot](http://www.gnuplot.info/)

A quick checklist for plots:
- Is there a caption explaining what is plotted?
- Are the axes clearly labelled? Do they have value ranges and units?
- Have you mentioned that this axis has logarithmic scale? (Logarithmic graphs
  are more fitting in some cases, but you should tell.)
- Is it clear which curve means what?
- Is it clear what are the measured points and what is an interpolated
  curve between them?
- Are there any overlaps? (E.g., the most interesting part of the curve
  hidden underneath a label?)

In your discussion, please distinguish the following kinds of claims.
It should be always clear which is which:
- Experimental results (i.e., the raw data you obtained from the experiments)
- Theoretical facts (i.e., claims we have proved mathematically)
- Your hypotheses (e.g., when you claim that the graph looks like something is true,
  but you are not able to prove rigorously that it always holds)

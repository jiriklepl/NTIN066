test: fibonacci_test
	./$<

CXXFLAGS=-std=c++11 -O2 -Wall -Wextra -g -Wno-sign-compare

fibonacci_test: fibonacci.h fibonacci_test.cpp test_main.cpp
	$(CXX) $(CXXFLAGS) fibonacci_test.cpp test_main.cpp -o $@

clean::
	rm -f fibonacci_test

.PHONY: clean test

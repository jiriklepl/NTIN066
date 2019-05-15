test: range_tree_test
	./$<

INCLUDE ?= .
CXXFLAGS=-std=c++11 -O2 -Wall -Wextra -g -Wno-sign-compare -I$(INCLUDE)

range_tree_test: range_tree_test.cpp range_tree.h test_main.cpp
	$(CXX) $(CXXFLAGS) $(filter %.cpp,$^) -o $@

clean:
	rm -f range_tree_test

.PHONY: clean test

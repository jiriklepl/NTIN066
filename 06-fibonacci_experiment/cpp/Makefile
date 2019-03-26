.PHONY: test
test: fibonacci_experiment
	@for exp in star random; do \
		for impl in std naive ; do \
			echo "t-$$exp-$$impl" ; \
			time ./fibonacci_experiment $$exp 42 $$impl >t-$$exp-$$impl ; \
		done ; \
	done
	@for b in biased-10 biased-40 biased-70 biased-100 ; do \
		echo "t-$$b" ; \
		time ./fibonacci_experiment $$b 42 std >t-$$b ; \
	done

INCLUDE ?= .
CXXFLAGS=-std=c++11 -O2 -Wall -Wextra -g -Wno-sign-compare -ggdb -I$(INCLUDE)

fibonacci_experiment: fibonacci.h fibonacci_experiment.cpp $(INCLUDE)/random.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) fibonacci_experiment.cpp -o $@

.PHONY: clean
clean:
	rm -f fibonacci_experiment

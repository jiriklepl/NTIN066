STUDENT_ID ?= 80

.PHONY: test
test: splay_experiment
	@rm -rf out && mkdir out
	@for test in sequential random subset ; do \
		for mode in std naive ; do \
			echo t-$$test-$$mode ; \
			/bin/time ./splay_experiment $$test $(STUDENT_ID) $$mode >out/t-$$test-$$mode ; \
		done ; \
	done

INCLUDE ?= .
CXXFLAGS=-std=c++17 -O2 -Wall -Wextra -g -Wno-sign-compare -I$(INCLUDE)

splay_experiment: splay_operation.h splay_experiment.cpp $(INCLUDE)/random.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f splay_experiment
	rm -rf out

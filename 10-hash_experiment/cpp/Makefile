INCLUDE ?= .
CXXFLAGS=-std=c++11 -O2 -Wall -Wextra -g -Wno-sign-compare -I$(INCLUDE)

hash_experiment: hash_experiment.cpp $(INCLUDE)/random.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) hash_experiment.cpp -o $@

.PHONY: clean
clean:
	rm -f hash_experiment

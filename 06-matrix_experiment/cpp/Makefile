.PHONY: test
test: matrix_experiment_sim matrix_experiment_real
	@for impl in smart naive ; do \
		echo "t-real-$$impl" ; \
		time ./matrix_experiment_real $$impl >out/t-real-$$impl ; \
	done
	@for impl in smart naive ; do \
		echo "t-real128-$$impl" ; \
		time ./matrix_experiment_real $${impl}128 >out/t-real128-$$impl ; \
	done

CXXFLAGS=-std=c++17 -O3 -Wall -Wextra -DNDEBUG -pedantic -march=native -s -Wno-sign-compare

matrix_experiment_sim: matrix_transpose.h matrix_tests.h matrix_experiment_sim.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) matrix_experiment_sim.cpp -o $@
	@for exp in m1024-b16 m8192-b64 m65536-b256 m65536-b4096 ; do \
		for impl in smart naive ; do \
			echo "t-sim-$$exp-$$impl" ; \
			./matrix_experiment_sim $$exp $$impl >out/t-sim-$$exp-$$impl ; \
		done ; \
	done

matrix_experiment_real: matrix_transpose_real.h matrix_tests.h matrix_experiment_real.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) matrix_experiment_real.cpp -o $@

.PHONY: clean
clean:
	rm -f matrix_experiment_sim matrix_experiment_real
	rm -rf out

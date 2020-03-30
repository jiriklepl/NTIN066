.PHONY: plot
test:
	@for exp in star random ; do \
		for impl in std naive ; do \
			echo "t-$$exp-$$impl" ; \
			time ./fibonacci_experiment.py $$exp 42 $$impl >t-$$exp-$$impl ; \
		done ; \
	done
	@for b in biased-10 biased-40 biased-70 biased-100 ; do \
		echo "t-$$b" ; \
		time ./fibonacci_experiment.py $$b 42 std >t-$$b ; \
	done


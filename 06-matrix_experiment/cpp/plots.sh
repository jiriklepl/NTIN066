#!/bin/sh

for naive in out/*naive; do
    naive=$(echo $naive | sed "s/[^/]*\///")
    smart=$(echo $naive | sed "s/naive/smart/")
    gnuplot -e "\
        set terminal pdf size 6,3; \
        set output \"${smart}_vs_${naive}.pdf\"; \
        set logscale x 2; \
        set xtics 4,2; \
        plot \"out/$smart\" with lines, \"out/$naive\" with lines;"
done

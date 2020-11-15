#!/usr/bin/awk -f

BEGIN {
    ls = "ls out/*naive"
    gnuplot = "gnuplot"
    FS = "-"
    print "set terminal pdf size 6,4" | gnuplot
    print "set logscale x 2" | gnuplot
    print "set logscale y 2" | gnuplot
    print "set yrange [2**(-10):1]" | gnuplot
    print "set xtics 1,2" | gnuplot
    print "set key bmargin" | gnuplot
    print "set key horizontal Left" | gnuplot
    print "set key left" | gnuplot
    print "set key reverse" | gnuplot
    while (ls | getline) {
        if ($2 == "sim") {
            m = int(substr($3,2))
            b = int(substr($4,2))
            b_ = b
            pow = 0
            while (b_ >= 1) {
                b_ /= 2
                pow++
            }
            print "set yrange [2**(-"pow"):1]" | gnuplot

            print "set output \"sim-m" m "-b" b ".pdf\"" | gnuplot

            smart = naive = $0
            sub("naive", "smart", smart)

            limit = 0.5 + 1 / (b * 2)
            plot = "plot \"" smart "\" title \"smart\" with linespoints"
            plot = plot ", \"" naive "\" title \"naive\" with linespoints"
            plot = plot ", (" limit ") title \"naive limit (" int(limit*1000)/1000 ")\""

            lower = 1 / b
            upper = lower * 3
            plot = plot ", (" lower ") title \"lower bound (" int(lower*1000)/1000 ")\""

            if (b > m / (2 * b)) {
                b_ = b
                while (b_ * b * 2 > m)
                    b_ /= 2

                approx = 1 /  b_
                plot = plot ", (" approx ") title \"upper bound of ideal (" int(approx*1000)/1000 ")\""
                upper = approx * 3
            }
            plot = plot ", (" upper ") title \"upper bound of non-ideal (" int(upper*1000)/1000 ")\""

            print plot | gnuplot
        } else {
        }
    }
    close(ls)
    close(gnuplot)
    # smart=$(echo $naive | sed "s/naive/smart/")
    # b=$(echo $naive | sed -n "s/.*b\([0-9]\+\).*/\1/p")$
    # if $(echo $naive | grep)
    # gnuplot -e "\
    #     set terminal pdf size 6,3; \
    #     set output \"${smart}_vs_${naive}.pdf\"; \
    #     set logscale x 2; \
    #     set xtics 4,2; \
    #     set key top left; \
    #     plot \"out/$smart\" title \"smart\" with linespoints, \
    #     \"out/$naive\" title \"naive\" with linespoints;"
}
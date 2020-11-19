#!/usr/bin/awk -f

BEGIN {
    ls = "ls out/*naive"
    gnuplot = "gnuplot"
    FS = "-"
    print "set terminal pdf size 6,4" | gnuplot
    print "set key bmargin" | gnuplot
    print "set key horizontal Left" | gnuplot
    print "set key left" | gnuplot
    print "set key reverse" | gnuplot
    print "set pointsize .7" | gnuplot
    print "set grid" | gnuplot
    logscale = -1

    while (ls | getline) {
        smart = naive = $0
        sub("naive", "smart", smart)

        if ($2 ~ /real/) {
            print "set autoscale" | gnuplot
        }

        if ($2 ~ /128/) {
            if (logscale != 0) {
                print "unset logscale x" | gnuplot
                print "set xrange [0:32768]" | gnuplot
                print "unset logscale y" | gnuplot
                print "set xtics 1024 rotate by -90" | gnuplot
                logscale = 0
            }
        } else if (logscale != 1) {
            print "set xtics 1,2 rotate by 0" | gnuplot
            print "set logscale x 2" | gnuplot
            print "set logscale y 2" | gnuplot
            logscale = 1
        }

        plot = "plot \"" smart "\" title \"smart\" with linespoints pt 6"
        plot = plot ", \"" naive "\" title \"naive\" with linespoints pt 6"

        if ($2 == "sim") {
            m = int(substr($3,2))
            b = int(substr($4,2))

            m_ = m
            m_pow = 0
            while (m_ > 1) {
                m_ /= 2
                m_pow++
            }

            b_ = b
            b_pow = 0
            while (b_ >= 1) {
                b_ /= 2
                b_pow++
            }

            print "set output \"sim-m" m "-b" b ".pdf\"" | gnuplot
            print "set xrange [2**("int(m_pow / 2)"):8192]" | gnuplot
            print "set yrange [2**(-"b_pow"):1]" | gnuplot
            print "set ylabel \"avg. cache-misses per access (=P)\"" | gnuplot
            print "set xlabel \"one-dimensional size of the matrix (=N)\"" | gnuplot


            limit = 0.5 + 1 / (b * 2)
            plot = plot ", (" limit ") title \"naive limit\""

            lower = 1 / b
            upper = b <= m / (4 * b) ? lower * 2 : lower * 3
            plot = plot ", (" lower ") title \"lower bound\""

            if (b > m / (2 * b)) {
                b_ = b
                while (b_ * b * 2 > m)
                    b_ /= 2

                approx = 1 /  b_
                plot = plot ", (" approx ") title \"upper bound of ideal\""
                upper = approx * 2
            }
            plot = plot ", (" upper ") title \"upper bound of non-ideal\""

            print plot | gnuplot
        } else if ($2 ~ /128/) {
            print "set output \""$2".pdf\"" | gnuplot
            print "set ylabel \"avg. time per item (=t [ns])\"" | gnuplot
            print "set xlabel \"one-dimensional size of the matrix (=N)\"" | gnuplot
            plot = "plot \"" smart "\" title \"smart\" with lines"
            plot = plot ", \"" naive "\" title \"naive\" with lines"
            plot = plot ", \"" naive "\" using (int($1) % 2048 == 1024 ? $1 : 1/0):2 title \"smart\" with points pt 6"
            plot = plot ", \"" naive "\" using (int($1) % 2048 == 0 ? $1 : 1/0):2 title \"smart\" with points pt 6"

            print plot | gnuplot
        } else {
            print "set output \""$2".pdf\"" | gnuplot
            print "set ylabel \"avg. time per item (=t [ns])\"" | gnuplot
            print "set xlabel \"one-dimensional size of the matrix (=N)\"" | gnuplot

            print plot | gnuplot
        }
    }
    close(ls)
    close(gnuplot)
}

#include "gnuplot.h"

gnuplot::gnuplot() {
    gnuplot_pipe = popen("gnuplot -persist", "w");
    if (!gnuplot_pipe) {
        std::cerr << "Error: gnuplot not found" << std::endl;
    }
}

gnuplot::~gnuplot() {
    pclose(gnuplot_pipe);
}

void gnuplot::operator()(const string &command) {
    fprintf(gnuplot_pipe, "%s\n", command.c_str());
    fflush(gnuplot_pipe);
}

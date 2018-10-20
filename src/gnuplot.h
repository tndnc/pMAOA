#include <string>
#include <iostream>

#ifndef PMAOA_GNUPLOT_H
#define PMAOA_GNUPLOT_H

using std::string;

class gnuplot {
public:
    gnuplot();
    ~gnuplot();

    void operator()(const string & command);
protected:
    FILE* gnuplot_pipe;
};

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

#endif //PMAOA_GNUPLOT_H

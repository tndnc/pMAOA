#include <string>
#include <iostream>

#ifndef PMAOA_GNUPLOT_H
#define PMAOA_GNUPLOT_H

using std::string;

class gnuplot {
public:
    gnuplot();
    ~gnuplot();

    void operator()(const string &command);
private:
    FILE* gnuplot_pipe;
};

#endif //PMAOA_GNUPLOT_H

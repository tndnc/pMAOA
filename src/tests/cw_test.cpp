#include <dirent.h>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iterative_descent.h>
#include <draw.h>

std::vector<std::string> getFileNames(const std::string &dirpath)
{
    DIR *dirp;
    struct dirent *directory;

    std::vector<std::string> filenames;
    dirp = opendir(dirpath.c_str());
    if (dirp) {
        while ((directory = readdir(dirp)) != nullptr) {
            std::string filename(directory->d_name);
            std::string tmp;
            std::istringstream iss(filename);
            while (getline(iss, tmp, '.')) {
                if (strncmp(tmp.c_str(), "vrp", 3) == 0) {
                    filenames.emplace_back(filename);
                }
            }
        }
        closedir(dirp);
    }

    return filenames;
}

int main() {
    auto filenames = getFileNames("../data/B/");
    for (auto &s : filenames) {
//        std::cout << "Starting C&W heuristic for: " << s << std::endl;
        maoa::Graph g("../data/B/" + s);
        std::list<maoa::cw::Saving> savings = maoa::cw::computeSavings(g);
        std::list<maoa::Tour> tours = maoa::cw::constructTours(g, savings);
//        std::cout << "Number of routes: " << tours.size() << std::endl;
        if (g.vehiclesNum() < tours.size()) {
            std::cerr << "Too many routes for " << s << " " << g.vehiclesNum() << " != " << tours.size() << std::endl;
//            maoa::idesc::descent(tours, g);
//            maoa::DrawUtils drawUtils;
//            drawUtils.drawTours(tours, g);
        }
    }
}
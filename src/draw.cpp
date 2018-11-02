#include "draw.h"

namespace maoa {

    void DrawUtils::drawGraph(Graph &g) {
        std::ofstream outfile;

        outfile.open("../points.dat");

        int nodeNum = g.nodeNum();
        int i;
        for (i = 0; i < nodeNum; i++) {
            maoa::NodeData data = g.getData(i);
            outfile << data.x << " " << data.y << std::endl;
        }

        outfile.close();

        plot("plot \"../points.dat\" pt 4 ps 1 lc rgb 'blue' notitle");
    }

    void DrawUtils::drawTours(std::list<maoa::Tour> &tours, maoa::Graph &g) {
        int colors[5][3] = {{233, 30, 99}, // red
                            {30, 150, 243}, // blue
                            {253, 216, 53}, // yellow
                            {67, 160, 71}, // green
                            {126, 87, 194}}; // purple

        std::ofstream outfile;
        outfile.open("../points.dat");

        maoa::NodeData depotData = g.getData(g.depotId());
        int color_idx = 0;
        for (maoa::Tour &t : tours) {
            outfile << depotData.x << ' ' << depotData.y << ' ' << colors[color_idx][0] << ' ' << colors[color_idx][1]
                    << ' ' << colors[color_idx][2] << std::endl;
            for (int city : t.cities) {
                maoa::NodeData data = g.getData(city);
                outfile << data.x << ' ' << data.y << ' ';
                outfile << colors[color_idx][0] << ' ' << colors[color_idx][1]
                        << ' ' << colors[color_idx][2] << std::endl;
            }
            outfile << depotData.x << ' ' << depotData.y << ' ' << colors[color_idx][0] << ' ' << colors[color_idx][1]
                    << ' ' << colors[color_idx][2] << std::endl;
            outfile << std::endl;
            color_idx++;
        }

        outfile.close();

        plot("rgb(r, g, b) = int(r)*65536 + int(g)*256 + int(b)");
        plot("plot \"../points.dat\" using 1:2:(rgb($3, $4, $5)) with linespoints pt 3 ps 1 lc rgb variable notitle");
    }
}

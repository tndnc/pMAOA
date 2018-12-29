#include "graph.h"

#ifndef PMAOA_ITERATIVE_DESCENT_H
#define PMAOA_ITERATIVE_DESCENT_H

namespace maoa {
namespace idesc {

    /*!
     * Returns the total distance between a string of cities, with the depot of
     * the graph \a g as starting and ending point.
     * @param t List of consecutive cities.
     * @param g Graph containing the cities of \a t.
     * @return The total distance.
     */
    double getTotalTourDistance(std::list<int> &t, const Graph &g);

    /*!
     * Returns the total distance of a solution, that is the sum of total tour
     * distances of the tours in the solution.
     * @param tours List of tours
     * @param g Graph containing the tours.
     * @return The total cost of the solution.
     */
    double getTotalCost(std::list<Tour> &tours, const Graph &g);

    /*!
     * Improves a list of tours by exploring the 2-opt neighborhood of each
     * tour. For each tour, the edges between four cities are deleted and the
     * cities are reconnected to construct a new and different tour. If the new
     * tour has a total distance inferior to the original distance, the new tour
     * is kept. The procedure ends when no 2-opt improvements can be made.
     * @param tours List of tours to improve.
     * @param g Graph containing the tours.
     * @return Boolean indicating if a change was made to any of the tours.
     */
    bool improve2opt(std::list<Tour> &tours, const Graph &g);

    /*!
     * Improves a list of tours by relocating a city from one tour to another.
     * For every city in the graph, the procedure tries to relocate it in a new
     * tour at the best location. The procedure ends when no improving
     * relocation can be made.
     * @param tours List of tours to improve.
     * @param g Graph containing the tours.
     * @return Boolean indicating if a change was made to any of the tours.
     */
    bool improveByRelocate(std::list<Tour> &tours, const Graph &g);

    /*!
     * Improves a list of tours by exchanging two cities between tours. For
     * every pair of city that are not in the same tour, if the exchange is
     * possible and improves the distance of the two corresponding tours, it is
     * implemented. The procedure ends when no improving exchange can be made.
     * @param tours List of tours to improve.
     * @param g Graph containing the tours.
     * @return Boolean indicating if a change was made to any of the tours.
     */
    bool improveByExchange(std::list<Tour> &tours, const Graph &g);

    /*!
     * Performs an iterative improvement procedure. The tours are improved with
     * several procedures in the following order: 2-opt neighborhood, relocating
     * of one city, exchange of city. The procedure ends when no improving
     * changes can be made.
     * @param tours List of tours to improve.
     * @param g Graph containing the tours.
     */
    void descent(std::list<Tour> &tours, const Graph &g);

} // namespace idesc
} // namespace maoa

#endif //PMAOA_ITERATIVE_DESCENT_H

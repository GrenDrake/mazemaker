#include <cstdlib>
#include <vector>

#include "map.h"
#include "makemaze.h"

void MazeMaker::step(Source source) {
    while (!points.empty()) {
        Coord here;
        unsigned pointIndex = -1;
        switch(source) {
            case Top:
                here = points.back();
                points.pop_back();
                break;
            case Random:
                pointIndex = rand() % points.size();
                here = points.at(pointIndex);
                points.erase(points.begin() + pointIndex);
                break;
            case Bottom:
                here = points.front();
                points.erase(points.begin());
                break;
            default:
                // invalid input
                return;
        }

        Direction starting = static_cast<Direction>(rand() % 4);
        Direction curDir = starting;
        map.at(here).viewed = true;
        do {
            Coord dest = here;
            dest.shift(curDir);
            if (map.valid(dest.x, dest.y) && !map.at(dest).viewed) {
                map.setWall(here.x, here.y, curDir, false);
                map.at(dest.x, dest.y).viewed = true;
                points.push_back(here);
                points.push_back(dest);
                return;
            }
            curDir = rotate(curDir);
        } while(curDir != starting);
    }
}

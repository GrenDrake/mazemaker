#include <cstdlib>
#include <vector>

#include "map.h"
#include "makemaze.h"

void MazeMaker::stepTop() {
    while (!points.empty()) {
        Coord here = points.back();
        points.pop_back();
        Direction starting = static_cast<Direction>(rand() % 4);
        Direction curDir = starting;
        map.at(here).visited = true;
        do {
            Coord dest = here;
            dest.shift(curDir);
            if (map.valid(dest.x, dest.y) && !map.at(dest).visited) {
                map.setWall(here.x, here.y, curDir, false);
                points.push_back(here);
                points.push_back(dest);
                return;
            }
            curDir = rotate(curDir);
        } while(curDir != starting);
    }
}

void MazeMaker::stepRandom() {
    while (!points.empty()) {
        unsigned index = rand() % points.size();
        Coord here = points.at(index);
        Direction starting = static_cast<Direction>(rand() % 4);
        Direction curDir = starting;
        map.at(here).visited = true;
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
        points.erase(points.begin() + index);
    }
}
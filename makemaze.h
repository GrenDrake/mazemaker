#ifndef MAKEMAZE_H
#define MAKEMAZE_H

#include <vector>

class Map;
struct Coord;

class MazeMaker {
public:
    MazeMaker(Map &map)
    : map(map)
    { }

    void setStart(const Coord &startAt) {
        if (points.empty())
            points.push_back(startAt);
    }
    void stepTop();
    void stepRandom();
    bool isDone() const {
        return points.empty();
    }
    void clear() {
        points.clear();
    }

private:
    Map &map;
    std::vector<Coord> points;
};

#endif

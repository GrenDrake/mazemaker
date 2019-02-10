#ifndef MAKEMAZE_H
#define MAKEMAZE_H

#include <vector>

class Map;
struct Coord;

class MazeMaker {
public:
    enum Source {
        Top, Random, Bottom
    };

    MazeMaker(Map &map)
    : map(map)
    { }

    void setStart(const Coord &startAt) {
        if (points.empty())
            points.push_back(startAt);
    }
    void step(Source source);
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

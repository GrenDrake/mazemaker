#include <ostream>

#include "map.h"

std::ostream& operator<<(std::ostream &out, Direction d) {
    switch(d) {
        case East: out << 'E'; break;
        case West: out << 'W'; break;
        case South: out << 'S'; break;
        case North: out << 'N'; break;
    }
    return out;
}

Direction rotate(Direction d) {
    switch(d) {
        case East:     return South;
        case South:    return West;
        case West:     return North;
        case North:    return East;
    }
}

std::ostream& operator<<(std::ostream &out, Coord c) {
    out << '(' << c.x << ',' << c.y << ')';
    return out;
}

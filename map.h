#ifndef MAP_H
#define MAP_H

#include <iosfwd>

enum Direction {
    North, East, South, West
};

struct Coord {
    Coord() : x(0), y(0) {}
    Coord(int x, int y) : x(x), y(y) {}

    void shift(Direction d) {
        switch(d) {
            case East:  ++x; break;
            case West:  --x; break;
            case South: ++y; break;
            case North: --y; break;
        }
    }

    bool operator==(const Coord &rhs) const {
        return rhs.x == x && rhs.y == y;
    }

    int x, y;
};

class Map {
public:
    const static int CELL_SIZE = 20;
    const static int RANGE = 1000;

    struct Cell {
        Cell()
        : east(true), south(true), dist(-1), visited(false), viewed(false)
        { }

        bool east, south;
        int dist;
        bool visited, viewed;
    };

    Map(int width, int height)
    : mWidth(width), mHeight(height), mTarget(-1,-1)
    {
        mData = new Cell[mWidth * mHeight];
    }
    ~Map() {
        delete[] mData;
    }

    int getWidth() const {
        return mWidth;
    }
    int getHeight() const {
        return mHeight;
    }

    void setOrigin(const Coord &origin) {
        mOrigin = origin;
    }
    const Coord& getOrigin() const {
        return mOrigin;
    }
    void setTarget(const Coord &target) {
        mTarget = target;
    }
    const Coord& getTarget() const {
        return mTarget;
    }

    void clearDist() {
        for (int i = 0; i < mWidth * mHeight; ++i) {
            mData[i].dist = -1;
        }
    }
    void clearAll() {
        mTarget.x = mTarget.y = -1;
        for (int i = 0; i < mWidth * mHeight; ++i) {
            mData[i] = Cell{};
        }
    }

    const Cell& at(const Coord &c) const {
        return at(c.x, c.y);
    }
    Cell& at(const Coord &c) {
        return at(c.x, c.y);
    }
    const Cell& at(int x, int y) const {
        return mData[x + y * mWidth];
    }
    Cell& at(int x, int y) {
        return mData[x + y * mWidth];
    }

    void setWall(const Coord &c, Direction dir, bool hasWall) {
        setWall(c.x, c.y, dir, hasWall);
    }
    void setWall(int x, int y, Direction dir, bool hasWall) {
        if (!valid(x,y)) return;
        switch(dir) {
            case East:  at(x,y).east = hasWall; break;
            case South: at(x,y).south = hasWall; break;
            case West:  if (valid(x-1,y)) at(x-1,y).east = hasWall; break;
            case North: if (valid(x,y-1)) at(x,y-1).south = hasWall; break;
        }
    }
    bool isOpen(const Coord &c, Direction dir) const {
        return isOpen(c.x, c.y, dir);
    }
    bool isOpen(int x, int y, Direction dir) const {
        if (!valid(x,y)) return false;
        switch(dir) {
            case East:  return !at(x,y).east;
            case South: return !at(x,y).south;
            case West:  return x == 0 ? false : !at(x-1,y).east;
            case North: return y == 0 ? false : !at(x,y-1).south;
        }
    }

    bool valid(const Coord &c) const {
        return c.x >= 0 && c.y >= 0 && c.x < mWidth && c.y < mHeight;
    }
    bool valid(int x, int y) const {
        return x >= 0 && y >= 0 && x < mWidth && y < mHeight;
    }

private:
    int mWidth, mHeight;
    Cell *mData;
    Coord mOrigin, mTarget;
};

std::ostream& operator<<(std::ostream &out, Direction d);
Direction rotate(Direction d);
std::ostream& operator<<(std::ostream &out, Coord c);

#endif

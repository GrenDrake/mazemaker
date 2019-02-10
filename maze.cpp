#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "makemaze.h"
#include "map.h"
#include "font.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;

enum class Display {
    None, Distance, Solution
};

struct Color {
    int r, g, b;
};

class MessageLog {
public:
    const static int MAX_MESSAGES = 3;

    void add(const std::string &message) {
        mLog[2] = mLog[1];
        mLog[1] = mLog[0];
        mLog[0] = message;
    }
    const std::string& get(int index) {
        if (index < 0 || index >= MAX_MESSAGES)
            return mInvalidIndexMsg;
        return mLog[MAX_MESSAGES - 1 - index];
    }
private:
    const static std::string mInvalidIndexMsg;
    std::string mLog[MAX_MESSAGES];
};
const std::string MessageLog::mInvalidIndexMsg = "(invalid index)";

Font font;
MessageLog messageLog;

Coord calcDistance(Map &map) {
    int maxDistance = -1;
    Coord mostDistant{-1, -1};
    std::vector<Coord> points;
    points.push_back(map.getOrigin());

    map.at(map.getOrigin()).dist = 0;
    while (!points.empty()) {
        Coord c = points.back();
        points.pop_back();
        int nextDist = map.at(c.x, c.y).dist + 1;
        for (int i = 0; i < 4; ++i) {
            Direction d = static_cast<Direction>(i);
            if (map.isOpen(c.x, c.y, d)) {
                Coord t = c;
                t.shift(d);
                if (map.at(t.x, t.y).dist < 0) {
                    map.at(t.x, t.y).dist = nextDist;
                    points.push_back(t);
                    if (nextDist > maxDistance) {
                        maxDistance = nextDist;
                        mostDistant = t;
                    }
                }
            }
        }
    }

    return mostDistant;
}

void findPath(Map &map) {
    Coord current = map.getTarget();
    while (map.at(current).dist > 0) {
        map.at(current).solution = true;
        Direction dir = North;
        Coord next(-1, -1);
        int cDistance = INT32_MAX;

        do {
            Coord target = current;
            target.shift(dir);
            if (map.isOpen(current, dir)) {
                int targetDistance = map.at(target).dist;
                if (targetDistance < cDistance) {
                    cDistance = map.at(target).dist;
                    next = target;
                }
            }
            dir = rotate(dir);
        } while (dir != North);

        if (next.x == -1 && next.y == -1) {
            return;
        } else {
            current = next;
        }
    }
}

bool tryMove(const Map &map, Coord &position, Direction dir, bool cheatMode) {
    if (cheatMode) {
        Coord target = position;
        target.shift(dir);
        if (map.valid(target.x, target.y)) {
            position = target;
            return true;
        }
        return false;
    } else {
        if (!map.isOpen(position.x, position.y, dir)) {
            return false;
        }
        position.shift(dir);
        return true;
    }
}

void drawMapCell(SDL_Renderer *renderer, int atX, int atY, const Map &map, const Coord &position, Display displayMode, bool player) {
    const Map::Cell &cell = map.at(position.x, position.y);
    const Map::Cell &target = map.at(map.getTarget());
    SDL_Rect here = { atX, atY, Map::CELL_SIZE, Map::CELL_SIZE };
    SDL_Rect distRect = { here.x + 4, here.y + 4, here.w - 8, here.h - 8 };

    if (position == map.getOrigin())
        SDL_SetRenderDrawColor(renderer, 255, 96, 255, SDL_ALPHA_OPAQUE);
    else if (position == map.getTarget())
        SDL_SetRenderDrawColor(renderer, 255, 255, 96, SDL_ALPHA_OPAQUE);
    else
        SDL_SetRenderDrawColor(renderer, 192, 192, 192, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &here);
    SDL_SetRenderDrawColor(renderer, 170, 170, 170, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &here);

    if (cell.dist >= 0 && displayMode == Display::Distance) {
        double percent = static_cast<double>(cell.dist) / static_cast<double>(target.dist);
        int greyness = 255.0 * percent;
        SDL_SetRenderDrawColor(renderer, greyness/2, greyness, greyness, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &distRect);
    }
    if (cell.solution && displayMode == Display::Solution) {
        SDL_SetRenderDrawColor(renderer, 128, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &distRect);
    }

    if (player) {
        SDL_SetRenderDrawColor(renderer, 255, 96, 96, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &distRect);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    if (!map.isOpen(position.x, position.y, East)) {
        SDL_Rect line = {
            atX + Map::CELL_SIZE - 1,
            atY,
            1, Map::CELL_SIZE };
        SDL_RenderFillRect(renderer, &line);
    }
    if (!map.isOpen(position.x, position.y, West)) {
        SDL_Rect line = {
            atX, atY,
            1, Map::CELL_SIZE };
        SDL_RenderFillRect(renderer, &line);
    }
    if (!map.isOpen(position.x, position.y, South)) {
        SDL_Rect line = {
            atX,
            atY + Map::CELL_SIZE - 1,
            Map::CELL_SIZE, 1 };
        SDL_RenderFillRect(renderer, &line);
    }
    if (!map.isOpen(position.x, position.y, North)) {
        SDL_Rect line = {
            atX,
            atY,
            Map::CELL_SIZE, 1 };
        SDL_RenderFillRect(renderer, &line);
    }

}

void gameloop(SDL_Renderer *renderer) {
    Map map(48, 30);
    MazeMaker mm(map);
    map.setOrigin(Coord{rand() % map.getWidth(), rand() % map.getHeight()});
    mm.setStart(map.getOrigin());
    Coord player = map.getOrigin();
    Coord target{ -1, -1 };

    unsigned mapSteps = 0;
    Display displayMode = Display::None;
    MazeMaker::Source mazeGen = MazeMaker::Random;
    bool cheatMode = false;
    const int X_OFFSET = 18;
    const int Y_OFFSET = 10;
    const int X_AXIS_OFFSET = (Map::CELL_SIZE - 16) / 2;
    const int Y_AXIS_OFFSET = (Map::CELL_SIZE - 8) / 2;
    const int MAX_DISPLAY_WIDTH = SCREEN_WIDTH / Map::CELL_SIZE - 2;
    const int MAX_DISPLAY_HEIGHT = (SCREEN_HEIGHT - 90) / Map::CELL_SIZE - 2;

    while (1) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        for (int x = 0; x < map.getWidth() && x < MAX_DISPLAY_WIDTH; ++x) {
            font.text(X_OFFSET + X_AXIS_OFFSET + x * Map::CELL_SIZE, 1, std::to_string(x + 1));
            font.text(X_OFFSET + X_AXIS_OFFSET + x * Map::CELL_SIZE, 11 + MAX_DISPLAY_HEIGHT * Map::CELL_SIZE, std::to_string(x + 1));
        }
        for (int y = 0; y < map.getHeight() && y < MAX_DISPLAY_HEIGHT; ++y) {
            font.text(1, Y_OFFSET + Y_AXIS_OFFSET + y * Map::CELL_SIZE, std::to_string(y + 1));
            font.text((MAX_DISPLAY_WIDTH + 1) * Map::CELL_SIZE,
                         Y_OFFSET + Y_AXIS_OFFSET + y * Map::CELL_SIZE, std::to_string(y + 1));
            for (int x = 0; x < map.getWidth() && x < MAX_DISPLAY_WIDTH; ++x) {
                // if (!map.valid(x,y)) continue;
                bool playerHere = false;
                if (player.x == x && player.y == y)
                    playerHere = true;
                drawMapCell(renderer,
                            X_OFFSET + x * Map::CELL_SIZE,
                            Y_OFFSET + y * Map::CELL_SIZE,
                            map, Coord(x, y),
                            displayMode, playerHere);
            }
        }

        font.text(0, SCREEN_HEIGHT - 70, messageLog.get(0));
        font.text(0, SCREEN_HEIGHT - 60, messageLog.get(1));
        font.text(0, SCREEN_HEIGHT - 50, messageLog.get(2));

        std::stringstream line1;
        line1 << "TAB - Toggle generator mode   Cursor: " << player;
        if (displayMode == Display::Distance) {
            line1 <<  "   To here: " << map.at(player.x, player.y).dist;
        } else if (displayMode != Display::None) {
            line1 <<  "   To target: " << map.at(map.getTarget()).dist;
        }
        line1 << "   Steps: " << mapSteps;
        font.text(0, SCREEN_HEIGHT - 30, line1.str());

        std::stringstream line2;
        line2 << "D - Display Mode   N - New Map   O - Warp to Origin   T Warp to Target   Mode: ";
        switch(mazeGen) {
            case MazeMaker::Top:    line2 << "top";     break;
            case MazeMaker::Random: line2 << "random";  break;
            case MazeMaker::Bottom: line2 << "bottom";  break;
        }
        font.text(0, SCREEN_HEIGHT - 20, line2.str());

        std::stringstream line3;
        line3 << "Z - Quit   SPACE - Step   R - Complete   C - Cheat mode   ARROWS - Move";
        if (cheatMode) line3 << "   CHEAT MODE";
        font.text(0, SCREEN_HEIGHT - 10, line3.str());

        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym) {
                        case SDLK_z:
                            return;

                        case SDLK_o:
                            player = map.getOrigin();
                            break;
                        case SDLK_t:
                            if (map.valid(map.getTarget()))
                                player = map.getTarget();
                            break;
                        case SDLK_c:
                            cheatMode = !cheatMode;
                            break;
                        case SDLK_d:
                            switch(displayMode) {
                                case Display::None:     displayMode = Display::Distance;    break;
                                case Display::Distance: displayMode = Display::Solution;    break;
                                default:                displayMode = Display::None;
                            }
                            break;
                        case SDLK_n:
                            map.clearAll();
                            target.x = target.y = -1;
                            mm.clear();
                            map.setOrigin(Coord{rand() % map.getWidth(), rand() % map.getHeight()});
                            mm.setStart(map.getOrigin());
                            player = map.getOrigin();
                            break;
                        case SDLK_TAB:
                            switch(mazeGen) {
                                case MazeMaker::Top:    mazeGen = MazeMaker::Random;    break;
                                case MazeMaker::Random: mazeGen = MazeMaker::Bottom;    break;
                                default:                mazeGen = MazeMaker::Top;       break;
                            }
                            break;
                        case SDLK_SPACE:
                            mm.step(mazeGen);
                            ++mapSteps;
                            if (mm.isDone()) {
                                map.clearDist();
                                map.setTarget(calcDistance(map));
                                findPath(map);
                                messageLog.add("Map completed.");
                            }
                            break;
                        case SDLK_r: {
                            std::clock_t start = std::clock();
                            while (!mm.isDone()) {
                                mm.step(mazeGen);
                                ++mapSteps;
                            }
                            std::clock_t end = std::clock();
                            double duration = (end - start) / static_cast<double>(CLOCKS_PER_SEC);
                            std::stringstream ss;
                            ss.precision(4);
                            ss << "Built in " << std::fixed << duration << " seconds.";
                            messageLog.add(ss.str());
                            map.clearDist();
                            map.setTarget(calcDistance(map));
                            findPath(map);
                            break; }

                        case SDLK_LEFT:     tryMove(map, player, West, cheatMode);     break;
                        case SDLK_RIGHT:    tryMove(map, player, East, cheatMode);     break;
                        case SDLK_UP:       tryMove(map, player, North, cheatMode);    break;
                        case SDLK_DOWN:     tryMove(map, player, South, cheatMode);    break;
                    }
                    break;
            }
        }
    }
}

int main() {
    srand(time(nullptr));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Title", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, 0);
    font.load(renderer, "font.bmp");

    gameloop(renderer);

    return 0;
}

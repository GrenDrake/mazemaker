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

struct Color {
    int r, g, b;
};

Font font;
std::vector<std::string> messageLog;

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

void drawMapCell(SDL_Renderer *renderer, int atX, int atY, const Map &map, const Coord &position, bool showDistances, bool player) {
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

    if (cell.dist >= 0 && showDistances) {
        double percent = static_cast<double>(cell.dist) / static_cast<double>(target.dist);
        int greyness = 255.0 * percent;
        SDL_SetRenderDrawColor(renderer, greyness/2, greyness, greyness, SDL_ALPHA_OPAQUE);
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
    Map map(40, 30);
    MazeMaker mm(map);
    map.setOrigin(Coord{rand() % map.getWidth(), rand() % map.getHeight()});
    mm.setStart(map.getOrigin());
    Coord player = map.getOrigin();
    Coord target{ -1, -1 };

    unsigned mapSteps = 0;
    bool showDistances = false;
    MazeMaker::Source mazeGen = MazeMaker::Random;
    bool cheatMode = false;
    const int X_OFFSET = 18;
    const int Y_OFFSET = 10;
    const int X_AXIS_OFFSET = (Map::CELL_SIZE - 16) / 2;
    const int Y_AXIS_OFFSET = (Map::CELL_SIZE - 8) / 2;
    while (1) {

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        for (int x = 0; x < map.getWidth() && x < 40; ++x) {
            font.text(X_OFFSET + X_AXIS_OFFSET + x * Map::CELL_SIZE, 1, std::to_string(x));
        }
        for (int y = 0; y < map.getWidth() && y < 30; ++y) {
            font.text(1, Y_OFFSET + Y_AXIS_OFFSET + y * Map::CELL_SIZE, std::to_string(y));
            for (int x = 0; x < map.getWidth() && x < 40; ++x) {
                // if (!map.valid(x,y)) continue;
                bool playerHere = false;
                if (player.x == x && player.y == y)
                    playerHere = true;
                drawMapCell(renderer,
                            X_OFFSET + x * Map::CELL_SIZE,
                            Y_OFFSET + y * Map::CELL_SIZE,
                            map, Coord(x, y),
                            showDistances, playerHere);
            }
        }

        unsigned pos = 0;
        for (unsigned i = messageLog.size(); i > 0 && pos < SCREEN_HEIGHT - 20; --i) {
            const unsigned idx = i - 1;
            font.text(820, pos - 10, messageLog[idx]);
            pos += 10;
        }

        std::stringstream line1;
        line1 << "TAB to Toggle Random/Last Generator Mode  PLAYER POS: " << player;
        if (showDistances) {
            line1 <<  "  TO HERE: " << map.at(player.x, player.y).dist;
            line1 <<  "  TO TARGET: " << map.at(map.getTarget()).dist;
        }
        line1 << "  STEPS: " << mapSteps;
        font.text(0, SCREEN_HEIGHT - 30, line1.str());

        std::stringstream line2;
        line2 << "D to Show/Hide Distances  N to Create New Map  O to Reset to Origin  T to Warp to Target";
        font.text(0, SCREEN_HEIGHT - 20, line2.str());

        std::stringstream line3;
        line3 << "Z to Quit   SPACE to Step  R to Run to Completion  C for Cheat Mode  ARROWS to Move";
        if (cheatMode) line3 << "  (CHEAT MODE)";
        switch(mazeGen) {
            case MazeMaker::Top:    line3 << "  (TOP)";     break;
            case MazeMaker::Random: line3 << "  (RANDOM)";  break;
            case MazeMaker::Bottom: line3 << "  (BOTTOM)";  break;
        }
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
                            showDistances = !showDistances;
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
                                messageLog.push_back("Map completed.");
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
                            ss << "Built in " << duration << " seconds.";
                            messageLog.push_back(ss.str());
                            map.clearDist();
                            map.setTarget(calcDistance(map));
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

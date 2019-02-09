# Maze Maker

Maze Maker is a simple program intended to experiment with the [Recursive Backtracker Algorithm](https://en.wikipedia.org/wiki/Maze_generation_algorithm) from generating mazes.

## Requirements

This program requires on SDL2, but has no other dependancies. Provided SDL2 is available, users on UNIX-like systems should be able to build the program with

```
make
```

and run it with

```
./maze
```

## Controls

When the program begins, the user is presented with a completed closed grid with a random origin point selected.

A maze can be generated either one step at a time or all the way through to completion. In the second case, the program will display the time spent generating the maze (not including any already generated areas).

Once the maze has been fully generated, the destination point is selected by finding the point the furthest distance from the origin. For each point, the distance between that point and the destination is calculated. The distance is displayed as cyan coloured dots that become darker the further the point is from the destination.

At any point, the user may navigate the generated maze using the arrow keys. By default, walls will prevent the cursor from moving in a given direction, but if "cheat mode" is activated, the user will be able to move through walls.

| Key        | Effect |
| ---------- | ------ |
| TAB        | Toggle between building from the last entered cell or any cell that's been entered. |
|  D         | Show and hide calculated distances. |
|  N         | Discard current map and generated a new closed grid and origin point. |
|  O         | Teleport the cursor to the origin point. |
|  T         | Teleport the cursor to the target. |
| SPACE      | Advance maze generation by one step. |
|  R         | Run the maze generator until the maze is completed. |
|  C         | Toggle "Cheat mode". |
| ARROW KEYS | Move around the generated maze. |

## License

This program is released under the [MIT license](https://opensource.org/licenses/MIT).

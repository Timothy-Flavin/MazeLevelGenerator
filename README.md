# MazeLevelGenerator
This project is a level generator for a maze game I am working on with a few friends. The goal of this generator is to create a room of a given width and height with a given number of randomly generated rooms and use the depth first search maze generation algorithm to create a dungeon for players to explore

## The Room Algorithm
The idea of the room generation algorithms is to take the number of rooms a user wants (Minimum size for each room is 4x4) and find the smallest perfect square above that number. The elgorithm then devides the map into said number of equal cells and then merge cells in groups of two until there is a number of cells remaining that is equal to the user requested number of rooms. This allows for rectangular randomly placed cells with no overlap and no sparse areas of the map. Inside each cell a room is generated leading to a maze with the number of rooms guarenteed without losing the organic randomness feel.

## Fixes/Innovations
Currently there is a rare bug due to the generation of doors. When rooms are generated they get 1-4 doors generated on them and if two rooms border eachother and they have a border of units long that is less than the number of doors generated between the two rooms, and all of those doors are generated on the wall the rooms share, the program can be stuck in an inifnite while loop. This is easy to solve with a check for remaining doors in the wall selection algorithm but I have not gotten around to it because this script will be translated to Node.js for use as a discord Bot

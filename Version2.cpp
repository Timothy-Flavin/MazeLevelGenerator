#include<iostream>
#include<random>
#include<ctime>

struct Cell {
	char *ascii; // ' ' by default, this could be the entity in the current cell like m for minotaur 
	bool visited = false; //this is for the maze generation algorithm. might want to add a seperate visited if you were doing visited in game
};
struct Room {
	int x1, y1, x2, y2, roomArea;
	bool merged = false;
};

void generateMaze(Cell* cells, const int WIDTH, const int HEIGHT, char* mapArray, int roomArea);
void printMaze(const int MAPWIDTH, const int MAPHEIGHT, char* mapArray);
void removeWall(int currentCell, int newCell, const int WIDTH, const int HEIGHT, char* mapArray);
int convertToMap(int cellNum, int WIDTH);
int generateRooms(const int LEVELWIDTH, const int LEVELHEIGHT, char* mapArray, int numRooms, Cell* cells);
int generateOrganicRooms(const int LEVELWIDTH, const int LEVELHEIGHT, char* mapArray, int numRooms, Cell* cells);
void clearRoom(int x1, int x2, int y1, int y2, const int WIDTH, char* mapArray);

int main() {
	//std::ios_base::sync_with_stdio(false);
	srand(time(NULL));
	const int LEVELWIDTH = 220;
	const int LEVELHEIGHT = 220;
	const int MAPARRAYWIDTH = 2 * LEVELWIDTH + 1;
	const int MAPARRAYHEIGHT = 2 * LEVELHEIGHT + 1;
	int playerX = 15, playerY = 15, numRooms = 69;

	char mapArray[MAPARRAYWIDTH*MAPARRAYHEIGHT];

	while (true) {
		for (int y = 0; y < MAPARRAYHEIGHT; y++) {
			for (int x = 0; x < MAPARRAYWIDTH; x++) {
				if (x % 2 == 0 && y % 2 == 0) {
					mapArray[y*MAPARRAYWIDTH + x] = '+';
				}
				else if (x % 2 == 0) {
					mapArray[y*MAPARRAYWIDTH + x] = '|';
				}
				else if (y % 2 == 0) {
					mapArray[y*MAPARRAYWIDTH + x] = '-';
				}
				else {
					mapArray[y*MAPARRAYWIDTH + x] = ' ';
				}
			}
		}

		Cell* cells = new Cell[LEVELWIDTH*LEVELHEIGHT];
		for (int y = 0; y < LEVELHEIGHT; y++) {
			for (int x = 0; x < LEVELWIDTH; x++) {
				cells[y*LEVELWIDTH + x].ascii = &mapArray[convertToMap(y*LEVELWIDTH + x, LEVELWIDTH)];
			}
		}

		*cells[playerY*LEVELWIDTH + playerX].ascii = 'P';
		int roomArea = generateRooms(LEVELWIDTH, LEVELHEIGHT, mapArray, numRooms, cells);
		generateMaze(cells, LEVELWIDTH, LEVELHEIGHT, mapArray, roomArea);
		printMaze(MAPARRAYWIDTH, MAPARRAYHEIGHT, mapArray);

		bool playing = true;
		char move = ' ';
		while (playing) {
			
			*cells[playerY*LEVELWIDTH + playerX].ascii = ' ';
			std::cin >> move;
			switch (move) {
			case 'w':
				if (mapArray[convertToMap(playerY*LEVELWIDTH + playerX, LEVELWIDTH)-MAPARRAYWIDTH] != '-') {
					playerY--;
					
				}
				break;
			case 'a':
				if (mapArray[convertToMap(playerY*LEVELWIDTH + playerX, LEVELWIDTH) - 1] != '|') {
					playerX--;
					
				}
				break;
			case 's':
				if (mapArray[convertToMap(playerY*LEVELWIDTH + playerX, LEVELWIDTH) + MAPARRAYWIDTH] != '-') {
					playerY++;
					
				}
				break;
			case 'd':
				if (mapArray[convertToMap(playerY*LEVELWIDTH + playerX, LEVELWIDTH) +1] != '|') {
					playerX++;
					
				}
				break;
			case 'x':
				playing = false;
				break;
			default:
				std::cout << "you messed up" << std::endl;
			}
			*cells[playerY*LEVELWIDTH + playerX].ascii = 'P';
			system("CLS");
			printMaze(MAPARRAYWIDTH, MAPARRAYHEIGHT, mapArray);

		}

		std::cin.get();
	}
}

void getNeighbors(int currentCell, int* numNeighbors, int* neighbors, Cell* cells, const int WIDTH, const int HEIGHT) {
	*numNeighbors = 0;
	if (currentCell - WIDTH > 0 && !cells[currentCell - WIDTH].visited) { //checks if the cell above is legal and unvisited
		neighbors[*numNeighbors] = currentCell - WIDTH;
		(*numNeighbors)++;
	}
	if (currentCell + WIDTH < WIDTH*HEIGHT && !cells[currentCell + WIDTH].visited) { //checks if the cell below is legal and unvisited
		neighbors[(*numNeighbors)] = currentCell + WIDTH;
		(*numNeighbors)++;
	}
	if ((currentCell + 1) % WIDTH != 0 && currentCell + 1 < WIDTH*HEIGHT && !cells[currentCell + 1].visited) { //checks if right neighbor is legal and unvisited
		neighbors[*numNeighbors] = currentCell + 1;
		(*numNeighbors)++;
	}
	if (currentCell%WIDTH != 0 && currentCell - 1>0 && !cells[currentCell - 1].visited) { //checks if left neighbor exists and is unvisited
		neighbors[*numNeighbors] = currentCell - 1;
		(*numNeighbors)++;
	}
}

void getPartitions(int currentCell, int* numNeighbors, int* neighbors, Room* rooms, const int WIDTH, const int HEIGHT) {
	*numNeighbors = 0;
	if (currentCell - WIDTH >= 0 && !rooms[currentCell - WIDTH].merged) { //checks if the cell above is legal and unvisited
		neighbors[*numNeighbors] = currentCell - WIDTH;
		(*numNeighbors)++;
	}
	if (currentCell + WIDTH < WIDTH*HEIGHT && !rooms[currentCell + WIDTH].merged) { //checks if the cell below is legal and unvisited
		neighbors[(*numNeighbors)] = currentCell + WIDTH;
		(*numNeighbors)++;
	}
	if ((currentCell + 1) % WIDTH != 0 && currentCell + 1 < WIDTH*HEIGHT && !rooms[currentCell + 1].merged) { //checks if right neighbor is legal and unvisited
		neighbors[*numNeighbors] = currentCell + 1;
		(*numNeighbors)++;
	}
	if (currentCell%WIDTH != 0 && currentCell - 1>=0 && !rooms[currentCell - 1].merged) { //checks if left neighbor exists and is unvisited
		neighbors[*numNeighbors] = currentCell - 1;
		(*numNeighbors)++;
	}
}

void removeWall(int currentCell, int newCell, const int WIDTH, const int HEIGHT, char* mapArray) {
	int smallerCell = currentCell < newCell ? currentCell : newCell;
	if (currentCell%WIDTH == newCell % WIDTH) { //if they are vertical neighbors
		mapArray[smallerCell+WIDTH] = ' ';
	}
	else if (currentCell == newCell + 2 || currentCell == newCell - 2) { // if they are horizontal neighbors
		mapArray[smallerCell+1] = ' ';
	}
	else {
		std::cout << "you fucked up" << std::endl;
	}
}

void generateMaze(Cell* cells, const int WIDTH, const int HEIGHT, char* mapArray, int roomArea) {
	int currentCell = -1;
	int* cellStack = new int[WIDTH*HEIGHT]; //making an array of size cells to store the index's of cells in a stack this wastes some memory but will never have to realocate or overflow
	int stackLength = 0; // initially empty
	int numVisited = 1;
	int neighbors[4]; //list of visitable neighbors to a cell
	int numNeighbors = 0; // number of visitable neighbors
	
	
	while (numNeighbors == 0) {
		currentCell++;
		getNeighbors(currentCell, &numNeighbors, neighbors, cells, WIDTH, HEIGHT);
		if (cells[currentCell].visited)
			numNeighbors = 0;
	}
	cells[currentCell].visited = true;
	//std::cout << currentCell<<std::endl;
	//std::cout << "numVisited: ";
	numVisited = 1;
	while (numVisited < WIDTH*HEIGHT - roomArea) {
		getNeighbors(currentCell, &numNeighbors, neighbors, cells, WIDTH, HEIGHT);
		if (numNeighbors != 0) {
			int newCell = neighbors[rand() % numNeighbors];
			if (numNeighbors > 1) {
				cellStack[stackLength] = currentCell;
				stackLength++;
			}
			removeWall(convertToMap(currentCell, WIDTH), convertToMap(newCell, WIDTH), 2*WIDTH+1, 2*HEIGHT+1, mapArray);
			currentCell = newCell;
			cells[currentCell].visited = true;
			numVisited++;
			//std::cout << ", " << numVisited;
		}
		else if (stackLength > 0) {
			int popCell = 0;
			bool noNeighbors = true;
			while (stackLength > 0 && noNeighbors) {
				popCell = cellStack[stackLength - 1];
				stackLength--;
				getNeighbors(popCell, &numNeighbors, neighbors, cells, WIDTH, HEIGHT);
				if (neighbors > 0) {
					noNeighbors = false;
				}
			}
			currentCell = popCell;
		}
	}
	std::cout << std::endl;

}

int generateOrganicRooms(const int WIDTH, const int HEIGHT, char* mapArray, int numRooms, Cell* cells) {
	int *roomX = new int[numRooms * 2];
	int *roomY = new int[numRooms * 2];
	int roomArea = 0;
	for (int i = 0; i < numRooms; i++) {
		
		bool notOverlapping = true;
		while (notOverlapping) {
			notOverlapping = false;
			roomX[i * 2] = rand() % WIDTH*2/3;
			roomX[i * 2 + 1] = roomX[i * 2] + rand() % ((WIDTH - roomX[i * 2])/4)+(WIDTH-roomX[i*2])/4;
			roomY[i * 2] = rand() % HEIGHT*2/3;
			roomY[i * 2 + 1] = roomY[i * 2] + rand() % ((HEIGHT - roomY[i * 2]) / 4)+(HEIGHT - roomY[i * 2])/4;
			for (int j = 0; j < i; j++) {
				if (roomX[i * 2] >= roomX[j*2] && roomX[i * 2] <= roomX[j*2 + 1] && roomY[i * 2] >= roomY[j*2] && roomY[i * 2] <= roomY[j*2 + 1]) {
					notOverlapping = true;
				}
				else if (roomX[i * 2+1] >= roomX[j * 2] && roomX[i * 2+1] <= roomX[j * 2 + 1] && roomY[i * 2+1] >= roomY[j * 2] && roomY[i * 2+1] <= roomY[j * 2 + 1]) {
					notOverlapping = true;
				}
				else if (roomX[i * 2] >= roomX[j * 2] && roomX[i * 2] <= roomX[j * 2 + 1] && roomY[i * 2 + 1] >= roomY[j * 2] && roomY[i * 2 + 1] <= roomY[j * 2 + 1]) {
					notOverlapping = true;
				}
				else if (roomX[i * 2 + 1] >= roomX[j * 2] && roomX[i * 2 + 1] <= roomX[j * 2 + 1] && roomY[i * 2] >= roomY[j * 2] && roomY[i * 2] <= roomY[j * 2 + 1]) {
					notOverlapping = true;
				}
				else if (roomX[2*i]<=roomX[2*j]&&roomY[2 * i]<=roomY[2 * j] && roomX[2 * i+1]>=roomX[2 * j+1] && roomY[2 * i+1]>=roomY[2 * j+1]) {
					notOverlapping = true;
				}
				else if (roomX[2 * i] >= roomX[2 * j] && roomY[2 * i] >= roomY[2 * j] && roomX[2 * i + 1] <= roomX[2 * j + 1] && roomY[2 * i + 1] <= roomY[2 * j + 1]) {
					notOverlapping = true;
				}
			}
		}
		//std::cout << "(x,y)1: " << roomX[i * 2] << ',' << roomY[i * 2] << std::endl;
		//std::cout << "(x,y)2: " << roomX[i * 2+1] << ',' << roomY[i * 2+1] << std::endl;
		for (int y = roomY[i*2]; y < roomY[i*2+1]; y++) {
			for (int x = roomX[i * 2]; x < roomX[i * 2 + 1]; x++) {
				if (cells[y*WIDTH + x].visited == false) {
					cells[y*WIDTH + x].visited = true;
					roomArea++;
				}
			}
		}
		clearRoom(roomX[i * 2], roomX[i * 2 + 1], roomY[i * 2], roomY[i * 2 + 1], WIDTH, mapArray);
		//roomArea += (roomX[i * 2 + 1] - roomX[i * 2])*(roomY[i * 2 + 1] - roomY[i * 2]);
	}
	return roomArea;
}

void clearRoom(int x1, int x2, int y1, int y2, const int WIDTH, char* mapArray) {
	int start = convertToMap(y1 * WIDTH + x1, WIDTH);
	int end = convertToMap((y2-1) * WIDTH + x2-1, WIDTH);
	//std::cout << "x1: " << x1 << ", y1: " << y1 << ", x2: " << x2 << ", y2: " << y2<<",   start: "<<x1*y1<<", end: "<<x2*y2<<", mapStart: "<<start<<", mapEnd: "<<end<<std::endl;
	for (int y = start / (2 * WIDTH + 1); y < end / (2 * WIDTH + 1)+1; y++) {
		for (int x = start % (2 * WIDTH + 1); x < end % (2 * WIDTH + 1)+1; x++) {
			mapArray[y*(2 * WIDTH + 1) + x] = ' ';
		}
	}
}

int generateRooms(const int WIDTH, const int HEIGHT, char* mapArray, int numRooms, Cell* cells) {
	int numPartitions = 1;
	int numMerges = 0;
	while (numPartitions * numPartitions < numRooms) {
		numPartitions++;
	}
	numMerges = numPartitions * numPartitions - numRooms;
	Room* initialRooms = new Room[numPartitions*numPartitions];
	Room* finalRooms = new Room[numRooms];
	for (int x = 0; x < numPartitions; x++) {
		for (int y = 0; y < numPartitions; y++) {
			initialRooms[y*numPartitions + x].x1 = WIDTH / numPartitions * x;
			initialRooms[y*numPartitions + x].x2 = WIDTH / numPartitions * (x + 1);
			initialRooms[y*numPartitions + x].y1 = HEIGHT / numPartitions * y;
			initialRooms[y*numPartitions + x].y2 = HEIGHT / numPartitions * (y + 1);
		}
	}
	for (int i = 0; i < numMerges; i++) {
		bool mergable = false;
		int room1 = 0, room2 = 0;
		int neighbors[4];
		int numNeighbors = 0;
		while (!mergable) {
			room1 = rand() % (numPartitions*numPartitions);
			getPartitions(room1, &numNeighbors, neighbors, initialRooms, numPartitions, numPartitions); //make this work
			
			if (numNeighbors > 0 && !(initialRooms[room1].merged)) {
				mergable = true;
				/*std::cout <<"currentRoom: "<<room1<<"numNeigbors:"<<numNeighbors<<std::endl;
				for (int j = 0; j < numNeighbors; j++) {
					std::cout << "neighbor " << j << ": " << neighbors[j] << std::endl;
				}*/
			}
		}
		room2 = neighbors[rand() % numNeighbors];
		//std::cout << "Room 2 chosen: " << room2 << std::endl;
		initialRooms[room1].merged = true;
		initialRooms[room2].merged = true;

		finalRooms[i].x1 = initialRooms[room1].x1 < initialRooms[room2].x1 ? initialRooms[room1].x1 : initialRooms[room2].x1;
		finalRooms[i].x2 = initialRooms[room1].x2 > initialRooms[room2].x2 ? initialRooms[room1].x2 : initialRooms[room2].x2;
		finalRooms[i].y1 = initialRooms[room1].y1 < initialRooms[room2].y1 ? initialRooms[room1].y1 : initialRooms[room2].y1;
		finalRooms[i].y2 = initialRooms[room1].y2 > initialRooms[room2].y2 ? initialRooms[room1].y2 : initialRooms[room2].y2;
		
	}
	int roomNum = numMerges;
	//std::cout << "room number: " << roomNum<<std::endl;
	for (int i = 0; i < numPartitions*numPartitions; i++) {
		if (!initialRooms[i].merged) {
			finalRooms[roomNum].x1 = initialRooms[i].x1;
			finalRooms[roomNum].x2 = initialRooms[i].x2;
			finalRooms[roomNum].y1 = initialRooms[i].y1;
			finalRooms[roomNum].y2 = initialRooms[i].y2;
			roomNum++;
		}
	}
	int areaTaken = 0;
	int numDoors = 0;
	int doors[4];
	int doorLocation = 0;
	for (int i = 0; i < numRooms; i++) {
		//std::cout << "final room x1: " << finalRooms[i].x1 << ", x2: " << finalRooms[i].x2 << ", y1: " << finalRooms[i].y1 << ", y2: " << finalRooms[i].y2<< std::endl;
		//std::cout << "clearing room: " << i << std::endl;
		finalRooms[i].x1 = finalRooms[i].x1 + rand() % (finalRooms[i].x2 - finalRooms[i].x1 - 4);
		finalRooms[i].x2 = finalRooms[i].x1 + rand() % (finalRooms[i].x2 - finalRooms[i].x1 - 3) + 4;
		finalRooms[i].y1 = finalRooms[i].y1 + rand() % (finalRooms[i].y2 - finalRooms[i].y1 - 4);
		finalRooms[i].y2 = finalRooms[i].y1 + rand() % (finalRooms[i].y2 - finalRooms[i].y1 - 3) + 4;

		numDoors = rand() % 4 + 1;
		std::cout << "number of doors [" << i << "]: " << numDoors<<std::endl;

		for (int j = 0; j < numDoors; j++) {
			do {
				doors[j] = rand() % 4;
			} while (doors[j] == 0 && finalRooms[i].y1 == 0 ||
				doors[j] == 1 && finalRooms[i].x2 == WIDTH ||
				doors[j] == 2 && finalRooms[i].y2 == HEIGHT ||
				doors[j] == 3 && finalRooms[i].x1 == 0);
			//create door on wall doors[j];
			switch (doors[j]) {
				case 0:
					do {
						doorLocation = finalRooms[i].x1 + rand() % (finalRooms[i].x2 - finalRooms[i].x1);
					} while (mapArray[convertToMap(finalRooms[i].y1 * WIDTH + doorLocation, WIDTH) - (2 * WIDTH + 1)] == ' ');
					mapArray[convertToMap(finalRooms[i].y1 * WIDTH + doorLocation, WIDTH) - (2 * WIDTH + 1)] = ' ';
					break;
				case 1:
					do {
						doorLocation = finalRooms[i].y1 + rand() % (finalRooms[i].y2 - finalRooms[i].y1);
					} while (mapArray[convertToMap(doorLocation * WIDTH + finalRooms[i].x2, WIDTH) - 1] == ' ');
					mapArray[convertToMap(doorLocation * WIDTH + finalRooms[i].x2, WIDTH) - 1] = ' ';
					break;
				case 2:
					do {
						doorLocation = finalRooms[i].x1 + rand() % (finalRooms[i].x2 - finalRooms[i].x1);
					} while (mapArray[convertToMap(finalRooms[i].y2 * WIDTH + doorLocation, WIDTH) + (2 * WIDTH + 1)] == ' ');
					mapArray[convertToMap(finalRooms[i].y2 * WIDTH + doorLocation, WIDTH) - (2 * WIDTH + 1)] = ' ';
					break;
				case 3:
					do {
						doorLocation = finalRooms[i].y1 + rand() % (finalRooms[i].y2 - finalRooms[i].y1);
					} while (mapArray[convertToMap(doorLocation * WIDTH + finalRooms[i].x1, WIDTH) - 1] == ' ');
					mapArray[convertToMap(doorLocation * WIDTH + finalRooms[i].x1, WIDTH) - 1] = ' ';
					break;
				default:
					std::cout << "you messed up the doors" << std::endl;
					break;
			}
		}

		clearRoom(finalRooms[i].x1, finalRooms[i].x2, finalRooms[i].y1, finalRooms[i].y2, WIDTH, mapArray);
		//areaTaken += (finalRooms[i].x2 - finalRooms[i].x1) * (finalRooms[i].y2 - finalRooms[i].y1);
		for (int y = finalRooms[i].y1; y < finalRooms[i].y2; y++) {
			for (int x = finalRooms[i].x1; x < finalRooms[i].x2; x++) {
				if (cells[y*WIDTH + x].visited == false) {
					cells[y*WIDTH + x].visited = true;
					areaTaken++;
				}
			}
		}
	}
	return areaTaken;
}

int convertToMap(int cellNum, int WIDTH) {
	return (cellNum / WIDTH * (2 * WIDTH + 1) * 2 + (2 * WIDTH + 1) + (cellNum%WIDTH) * 2 + 1);
}

int convertToCell() {
	return 0;
}

void printMaze(const int MAPWIDTH, const int MAPHEIGHT, char* mapArray) {
	for (int y = 0; y < MAPHEIGHT; y++) {
		for (int x = 0; x < MAPWIDTH; x++) 
			std::cout << mapArray[y*MAPWIDTH + x] << (mapArray[y*MAPWIDTH + x] != '-' && mapArray[y*MAPWIDTH + x + 1] != '-' &&y*MAPWIDTH + x + 1 <MAPHEIGHT*MAPWIDTH ? ' ' : (x<MAPWIDTH - 1 ? (y % 2 == 0 ? '-' : ' ') : ' '));
		std::cout << std::endl;
	}
}

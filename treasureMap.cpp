#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s)
{
	start = s;
	base = baseim;
	maze = mazeim;
}

	// changes the pixel at position loc to consist of colour channel
    // values which are 50% of their original. Note that because of 
    // our encoding scheme which essentially invalidates the lower
    // order bits, you should compute each value as 2*(x/4), where
    // x is the integer value of a colour channel.

void treasureMap::setGrey(PNG & im, pair<int,int> loc){
	RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
	pixel->r = 2*(pixel->r/4);
	pixel->g = 2*(pixel->g/4);
	pixel->b = 2*(pixel->b/4);
}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d){
	int dmod = d % 64;
	RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
	//not inlined for readabilty
	unsigned char redEncoding = (dmod >> 4) & 0x3; 
	unsigned char greenEncoding = (dmod >> 2) & 0x3;
	unsigned char blueEncoding = dmod & 0x3; 
	pixel->r = (pixel->r & 0xFC) | redEncoding;
	pixel->g = (pixel->g & 0xFC) | greenEncoding;
	pixel->b = (pixel->b & 0xFC) | blueEncoding;
}

void treasureMap::drawRedBox(PNG & im, pair<int,int> point) {
	unsigned int rowIndex = point.second - 3 >= 0 ? point.second - 3 : 0;
	unsigned int colIndex = point.first - 3 >= 0 ? point.first - 3 : 0;
	unsigned int endRowIndex = (point.second + 3 < im.height()) ? point.second + 3 : im.height() - 1;
	unsigned int endColIndex = (point.first + 3 < im.width()) ? point.first + 3 : im.width() - 1;
/*
	if (rowIndex == 0) {
		endRowIndex = 6;
	} else if (endRowIndex == im.height() - 1) {
		rowIndex = im.height() - 7;
	}

	if (colIndex == 0) {
		endColIndex = 6;
	} else if (endColIndex == im.width() - 1) {
		colIndex = im.width() - 7;
	}
*/
	int colReset = colIndex;
	while (rowIndex <= endRowIndex) {
		while (colIndex <= endColIndex) {
			RGBAPixel *pixel = im.getPixel(colIndex, rowIndex);
			pixel->r = 255;
			pixel->g = 0;
			pixel->b = 0;
			colIndex++;
		}
		colIndex = colReset;
		rowIndex++;
	}
}

PNG treasureMap::renderMap(){
	PNG EncodedMap = base;
	vector<vector<bool>> visited(base.height(), vector<bool>(base.width(), false)); 
	vector<vector<int>> location(base.height(), vector<int>(base.width(), -1));
	Queue<pair<int,int>> BFStraversal;

	visited[start.second][start.first] = true;
	location[start.second][start.first] = 0;
	setLOB(EncodedMap, start, location[start.second][start.first]);
	BFStraversal.enqueue(start);

	while(!BFStraversal.isEmpty()) {
		pair<int,int> curr = BFStraversal.dequeue();
		vector<pair<int,int>> adjacencyList = neighbors(curr);
		for (unsigned long i = 0; i < adjacencyList.size(); i++) {
			pair<int,int> adjacentPos = adjacencyList[i]; //For readability, can be inlined if required
			if (good(visited, curr, adjacentPos)) {
				visited[adjacentPos.second][adjacentPos.first] = true;
				location[adjacentPos.second][adjacentPos.first] = location[curr.second][curr.first] + 1;
				setLOB(EncodedMap, adjacentPos, location[adjacentPos.second][adjacentPos.first]);
				BFStraversal.enqueue(adjacentPos);
			}
		}
	}
	return EncodedMap;

}


PNG treasureMap::renderMaze(){
	PNG MazeMap = base;
	vector<vector<bool>> visited(base.height(), vector<bool>(base.width(), false)); 
	Queue<pair<int,int>> BFStraversal;

	visited[start.second][start.first] = true;
	setGrey(MazeMap, start);
	BFStraversal.enqueue(start);

	while(!BFStraversal.isEmpty()) {
		pair<int,int> curr = BFStraversal.dequeue();
		vector<pair<int,int>> adjacencyList = neighbors(curr);
		for (unsigned long i = 0; i < adjacencyList.size(); i++) {
			pair<int,int> adjacentPos = adjacencyList[i]; //For readability, can be inlined if required
			if (good(visited, curr, adjacentPos)) {
				visited[adjacentPos.second][adjacentPos.first] = true;
				setGrey(MazeMap, adjacentPos);
				BFStraversal.enqueue(adjacentPos);
			}
		}
	}

	drawRedBox(MazeMap, start);
	return MazeMap;

}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next){

	bool isWithinImage = (next.first < maze.width()) && (next.first >= 0) && (next.second < maze.height()) && (next.second >= 0);
	if (!isWithinImage) {
		return false;
	}
	bool isNotVisited = !(v[next.second][next.first]);
	RGBAPixel *currColour = maze.getPixel(curr.first, curr.second);
	RGBAPixel *nextColour = maze.getPixel(next.first, next.second);
	bool isSameColour = (*currColour == *nextColour);
	return isWithinImage && isNotVisited && isSameColour;


}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {
	vector<pair<int,int>> neighbors;
	neighbors.push_back(make_pair(curr.first - 1, curr.second));
	neighbors.push_back(make_pair(curr.first, curr.second + 1));
	neighbors.push_back(make_pair(curr.first + 1, curr.second));
	neighbors.push_back(make_pair(curr.first, curr.second - 1));
	return neighbors;
}


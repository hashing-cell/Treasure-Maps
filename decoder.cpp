#include "decoder.h"
#include "queue.h"
#include "stack.h"
using namespace std;

    // initializes variables and build solution path as a vector
    // of points that corresponds to a sequence of pixels 
    // beginning at pathPts[0] == start, and ending with
    // pathPts[pathPts.size() - 1] == end. 
    //
    // You will want to modify the BFS algorithm you wrote for the
    // treasureMap renderer in order to solve this problem.
    //
    // As a hint, you will want an additional working vector whose purpose
    // is to encode, for every location, the location from which it
    // was first discovered. This working structure will help you 
    // determine the path from any location to the start (and thus
    // from the start to any location). 

decoder::decoder(const PNG & tm, pair<int,int> s)
   :start(s),mapImg(tm) {


	PNG EncodedMap = tm;
	vector<vector<bool>> visited(tm.height(), vector<bool>(tm.width(), false)); 
	vector<vector<int>> location(tm.height(), vector<int>(tm.width(), -1));
	vector<vector<pair<int,int>>> prevLocation(tm.height(), vector<pair<int,int>>(tm.width(), make_pair(-2,-2)));
	Queue<pair<int,int>> BFStraversal;

	visited[start.second][start.first] = true;
	location[start.second][start.first] = 0;
	BFStraversal.enqueue(start);

	pair<int,int> curr;
	while(!BFStraversal.isEmpty()) {
		curr = BFStraversal.dequeue();
		vector<pair<int,int>> adjacencyList = neighbors(curr);
		for (unsigned long i = 0; i < adjacencyList.size(); i++) {
			pair<int,int> adjacentPos = adjacencyList[i]; //For readability, can be inlined if required
			if (good(visited, location, curr, adjacentPos)) {
				visited[adjacentPos.second][adjacentPos.first] = true;
				location[adjacentPos.second][adjacentPos.first] = location[curr.second][curr.first] + 1;
				prevLocation[adjacentPos.second][adjacentPos.first] = curr;
				BFStraversal.enqueue(adjacentPos);
			}
		}
	}

	Stack<pair<int,int>> reverseTraversal;

	for (pair<int,int> next = curr; next != start; next = prevLocation[next.second][next.first]) {
		reverseTraversal.push(next);
	}
    reverseTraversal.push(start);

    while(!reverseTraversal.isEmpty()) {
    	pathPts.push_back(reverseTraversal.pop());
    }

}
	
void decoder::drawRedBox(PNG & im, pair<int,int> point) {
	unsigned int rowIndex = point.second - 3 >= 0 ? point.second - 3 : 0;
	unsigned int colIndex = point.first - 3 >= 0 ? point.first - 3 : 0;
	unsigned int endRowIndex = (point.second + 3 < im.height()) ? point.second + 3 : im.height() - 1;
	unsigned int endColIndex = (point.first + 3 < im.width()) ? point.first + 3 : im.width() - 1;

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

PNG decoder::renderSolution(){
	PNG solution = mapImg; 
	for (unsigned int i = 0; i < pathPts.size(); i++) {
		pair<int,int> currPt = pathPts[i];
		RGBAPixel *pixel = solution.getPixel(currPt.first, currPt.second);
		pixel->r = 255;
		pixel->g = 0;
		pixel->b = 0;
	}
	return solution;

}

PNG decoder::renderMaze(){
	PNG MazeMap = mapImg;
	vector<vector<bool>> visited(mapImg.height(), vector<bool>(mapImg.width(), false)); 
	vector<vector<int>> location(mapImg.height(), vector<int>(mapImg.width(), -1));
	Queue<pair<int,int>> BFStraversal;

	visited[start.second][start.first] = true;
	location[start.second][start.first] = 0;
	setGrey(MazeMap, start);
	BFStraversal.enqueue(start);

	while(!BFStraversal.isEmpty()) {
		pair<int,int> curr = BFStraversal.dequeue();
		vector<pair<int,int>> adjacencyList = neighbors(curr);
		for (unsigned long i = 0; i < adjacencyList.size(); i++) {
			pair<int,int> adjacentPos = adjacencyList[i]; //For readability, can be inlined if required
			if (good(visited, location, curr, adjacentPos)) {
				visited[adjacentPos.second][adjacentPos.first] = true;
				location[adjacentPos.second][adjacentPos.first] = location[curr.second][curr.first] + 1;
				setGrey(MazeMap, adjacentPos);
				BFStraversal.enqueue(adjacentPos);
			}
		}
	}
	drawRedBox(MazeMap, start);
	return MazeMap;


}

void decoder::setGrey(PNG & im, pair<int,int> loc){
	RGBAPixel *pixel = im.getPixel(loc.first, loc.second);
	pixel->r = 2*(pixel->r/4);
	pixel->g = 2*(pixel->g/4);
	pixel->b = 2*(pixel->b/4);

}

pair<int,int> decoder::findSpot(){
	return pathPts[pathPts.size() - 1];

}

int decoder::pathLength(){
	return pathPts.size();
}

	// tests a neighbor (adjacent vertex) to see if it is 
    // 1. within the image, 2. unvisited, and 3. coloured so that
    // lower order bits fit the pattern prescribed by the encoder.
    // An entry in table v is true if a cell has previously been 
    // visited. table d contains the shortest distance from each location
    // back to the start. the (r,g,b) colour of position next must have lower order
    // bits (XX,XX,XX) that, when interpreted as an integer between 0 and 63,
    // inclusive, is d[curr.second][curr.first] + 1 (mod 64).
bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next){
	bool isWithinImage = (next.first < mapImg.width()) && (next.first >= 0) && (next.second < mapImg.height()) && (next.second >= 0);
	if (!isWithinImage) {
		return false;
	}
	bool isNotVisited = !(v[next.second][next.first]);
	RGBAPixel *nextColour = mapImg.getPixel(next.first, next.second);
	bool isCorrectEncoding = compare(*nextColour, d[curr.second][curr.first]);
	return isWithinImage && isNotVisited && isCorrectEncoding;

}

vector<pair<int,int>> decoder::neighbors(pair<int,int> curr) {
	vector<pair<int,int>> neighbors;
	neighbors.push_back(make_pair(curr.first - 1, curr.second));
	neighbors.push_back(make_pair(curr.first, curr.second + 1));
	neighbors.push_back(make_pair(curr.first + 1, curr.second));
	neighbors.push_back(make_pair(curr.first, curr.second - 1));
	return neighbors;
}


bool decoder::compare(RGBAPixel p, int d){
	return ((p.r % 4) * 16 + (p.g % 4) * 4 + (p.b % 4)) == (d + 1) % 64;
}

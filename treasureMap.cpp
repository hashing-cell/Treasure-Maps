#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s)
{
	start = s;
	base = baseim;
	maze = mazeim;
}

void treasureMap::setGrey(PNG & im, pair<int,int> loc){

/* YOUR CODE HERE */

}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d){

/* YOUR CODE HERE */

}
// draws the entire maze on a copy of the base image inside the low order bits.
    // in this encoding, each pixel differs by very little from its 
    // original value. The returned PNG will be visually identical to the base,
    // but anyone with a decoder can find and solve the maze. (We specify the
    // computation of the encoded colours in the description of a setLOB function,
    // below.)
    //
    // This function does a "breadth-first" traversal of the maze image, and at each
    // iteration of the traversal, it encodes the current location as a part of the
    // maze in a copy of the base image. The algorithm is spelled out carefully
    // for this function. You will want to adapt it to design solutions
    // to some of the other functions in this assignment.
    //
    // 0. Make a copy of the base image. You will change its pixels corresponding to 
    //      locations within the maze.
    // 1. Initialize working vectors: 
    //      a. Define a 2d vector whose dimensions are the same as the image, and whose 
    //      values are boolean. Use this vector to indicate whether or not a location
    //      has been visited by the traversal.
    //      b. Define a 2d vector whose dimensions are the same as the image, and whose
    //      values are integers. This structure will be used to store the length of the
    //      shortest path between the current location and the start location.
    // 2. Initialize a queue whose purpose is to orchestrate the traversal. As you search
    //      the maze, you will have to keep a list of locations to explore. The queue maintains
    //      that list.
    // 3. For the start location, do the following:
    //      a. mark the start location as visited (structure in step 1a)
    //      b. set the distance to the start to be 0 (structure in step 1b)
    //      c. encode the distance within the start location's lower order bits of the treasure map 
    //          (structure in step 0)
    //      d. enqueue the start location. (structure in step 2)
    // 4. While the queue is not empty:
    //      a. set the current location, curr, to the value at the front of the queue (and dequeue). 
    //      b. for each compass neighbor, p, of curr in the order of left, below, right, above:
    //          i. if p is "good" (defined below) then
    //              1) mark p's location as visited
    //              2) set p's distance to the start to be curr's distance, + 1. 
    //              3) encode the distance in the lower order bits of the treasure map
    //              4) enqueue p
    //      
    // 5. return the image
PNG treasureMap::renderMap(){

/* YOUR CODE HERE */

}


PNG treasureMap::renderMaze(){

/* YOUR CODE HERE */

}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next){
	/*
		Can be inlined at the cost of readability (ie. getting the longest return statement ever)
	*/

	bool isWithinImage = (next.first < base.width()) && (next.first >= 0) && (next.second < base.height()) && (next.second >= 0);
	bool isVisited = !(v[next.second][next.first]);
	RGBAPixel *currColour = base.getPixel(curr.first, curr.second);
	RGBAPixel *nextColour = base.getPixel(next.first, next.second);
	bool isSameColour = (*currColour == *nextColour);
	return isWithinImage && isVisited && isSameColour;


}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {
	vector<pair<int,int>> neighbors;
	neighbors.push_back(make_pair(curr.first - 1, curr.second));
	neighbors.push_back(make_pair(curr.first, curr.second + 1));
	neighbors.push_back(make_pair(curr.first + 1, curr.second));
	neighbors.push_back(make_pair(curr.first, curr.second - 1));
	return neighbors;
}


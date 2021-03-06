#ifndef _BUILDING_H_
#define _BUILDING_H_

#include <cmath>
#include <vector>
#include "boundingbox.h"
#include "lot.h"

class ArgParser;

// Simple edge class used for filling in building footprints 
class Edge {
public:
  glm::vec3 start;
  glm::vec3 end;
  
  Edge(const glm::vec3& a, const glm::vec3& b) { start = a; end = b; }
};

//------------------------------------------------------------------

// discretised grid that represents the bounds of a building
// and also contains information about how its layers are filled
class BoundingGrid {
private:
  int min_x, min_z;
  int max_x, max_z;
  int min_y, max_y;
  
  // the footprint of whatever is occupying the grid
  std::vector< std::vector<bool> > layers;
  
public:
  // create a trivial bounding grid of zero area at the origin
  BoundingGrid() { min_x = min_z = max_x = max_z = 0; min_y = max_y = 0; };
  // create a single square bounding grid containing start position
  BoundingGrid( glm::vec3 pos ) {
    min_x = floor(pos.x); min_z = floor(pos.z);
    max_x = ceil(pos.x); max_z = ceil(pos.z);
    min_y = floor(pos.y); max_y = ceil(pos.y);
  }
  glm::vec3 getMin() { return glm::vec3(min_x, min_y, min_z); }
  glm::vec3 getMax() { return glm::vec3(max_x, max_y, max_z); }
  glm::vec3 getDiff() { return getMax()-getMin(); }
  
  // makes sure a given point fits in the bounding grid
  void extend( const glm::vec3& pos ) {
	// if x or z is past the lower bound of the grid
	// extend the grid backwards to fit it
	float x = floor(pos.x); float z = floor(pos.z);
	if ( x < min_x ) min_x = x; if ( z < min_z ) min_z = z;
	// if x or z is past the upper bound of the grid
	// extend the grid forwards to fit it
	x = ceil(pos.x); z = ceil(pos.z);
	if ( x > max_x ) max_x = x; if ( z > max_z ) max_z = z;
	// also extend in y direction
	float y = floor(pos.y); if ( y < min_y ) min_y = y;
		  y =  ceil(pos.y); if ( y > max_y ) max_y = y;
  }
  
  // used in determination of an object's footprint
  void resizeLayers() { 
	  layers.resize(max_y-min_y);
	  for ( int j = 0; j < (max_y - min_y); j++ ) {
		  layers[j].resize((max_x-min_x)*(max_z-min_z), false );
	  }
  }
  
  void mark(const Edge& e);
  
  // translation to 1D indices in footprint vector
  int map2Dto1D( int i, int k ) const { return k*(max_x-min_x) + i - min_x; }
  
  // checks whether the object takes up space in a specific square
  bool checkFootprint( int i, int k ) const { return layers[0][map2Dto1D(i,k)]; }
  bool queryVolume( int i, int j, int k ) const { return layers[j][map2Dto1D(i,k)]; }
  
  void print();
};

//----------------------------------------------------------------------

// data structure that represents a building object
class Building {

friend class Grid;

private:
  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> faces;
  std::vector<Edge> edges;
  glm::vec4 color;
  //bounding box - based on actual coordinate space
  BoundingBox bbox;
  //BoundingBox bbox;
  BoundingGrid bgrid;
  // size parameters
  int width;
  int height;
  int length;
  int size;
  
public:
  Building(ArgParser* args, unsigned int index);
  
  //int getSize() const {return size;}
  //building attribute shuffling functions
  void sizeShuffle(unsigned int max, unsigned int min);
  void colorShuffle();
  
  // size accessors
  int getWidth() { return width; }
  int getLength() { return length; }
  int getHeight() { return height; }
  int getSize() {
	int size = 0;
	for ( int i = 0; i < width; i++ ) {
	  for ( int k = 0; k < length; k++ ) {
		if ( checkFootprint(i,k) ) size += 1;
	  }
	}
	return size;
  }
  
  // check specific slot of the building footprint
  bool checkFootprint(int i, int k) const {
	return bgrid.checkFootprint(i,k);
  }
  bool queryVolume(int i, int j, int k) const {
	return bgrid.queryVolume(i,j,k);
  }
  
  void print() {
	  bgrid.print();
  }
};

#endif

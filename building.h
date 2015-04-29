#include <cmath>
#include <vector>
#include "boundingbox.h"

class ArgParser;

class Edge {
public:
  glm::vec3 start;
  glm::vec3 end;
  
  Edge(const glm::vec3& a, const glm::vec3& b) { start = a; end = b; }
};

class BoundingGrid {
private:
  int min_x, min_z;
  int max_x, max_z;
  std::vector<bool> footprint;
public:
  BoundingGrid() { min_x = min_z = max_x = max_z = 0; };
  BoundingGrid( glm::vec3 pos ) {
    min_x = floor(pos.x); min_z = floor(pos.z);
    max_x = ceil(pos.x); max_z = ceil(pos.z);
  }
  glm::vec3 getMin() { return glm::vec3(min_x, 1, min_z); }
  glm::vec3 getMax() { return glm::vec3(max_x, 1, max_z); }
  glm::vec3 getDiff() { return getMax()-getMin(); }
  
  void extend( const glm::vec3& pos ) {
	float x = floor(pos.x); float z = floor(pos.z);
	if ( x < min_x ) min_x = x; if ( x > max_x ) max_x = x;
	if ( z < min_z ) min_z = z; if ( z > max_z ) max_z = z;
  }
  
  void resizeFootprint() { footprint.resize( (max_x - min_x) * (max_z - min_z), false ); }
  
  void mark(const Edge& e);
  
  int map2Dto1D( int i, int k ) const { return k*(max_x-min_x) + i; }
  
  void print();
};

class Building {
	
friend class Grid;

private:
  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> faces;
  std::vector<Edge> edges;
  glm::vec4 color;
  BoundingBox bbox;
  BoundingGrid bgrid;
  int width;
  int height;
  int length;
  
public:
  Building(ArgParser* args);
  //visual shuffling functions
  void sizeShuffle();
  void colorShuffle();
  void rotationShuffle();
};



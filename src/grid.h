#ifndef _GRID_H_
#define _GRID_H_

#include <cassert>
#include <vector>
#include <queue>

#include "argparser.h"
#include "boundingbox.h"
#include "vbo_structs.h"
#include "building.h"
#include "lot.h"

//comparison class for sorting PQ
class bldgCompare{
	public:
	bool operator() (Building left , Building right) const
    {
	  return left.getSize() < right.getSize();
    }
};

typedef  std::priority_queue<Building,std::vector<Building>,bldgCompare> bldg_PQ;

class Grid {
private:
  ArgParser* args;
  int width = 1, length = 1, height = 1;
  BoundingBox bbox;
  bldg_PQ bldgsPQ;
  std::vector<Building> bldgs;
  std::vector< std::vector<Lot> > lots;
  
  // grid geometry information
  std::vector<VBOPosNormalColor> lot_tri_verts;
  std::vector<VBOIndexedTri> lot_tri_indices;
  std::vector<VBOPosNormalColor> grid_edge_verts;
  std::vector<VBOIndexedTri> grid_edge_indices;
  
  // building geometry information
  std::vector<VBOPosNormalColor> bldg_tri_verts;
  std::vector<VBOIndexedTri> bldg_tri_indices;
  
  // VBOs
  GLuint lot_tri_verts_VBO;
  GLuint lot_tri_indices_VBO;
  GLuint grid_edge_verts_VBO;
  GLuint grid_edge_indices_VBO;
  GLuint bldg_tri_verts_VBO;
  GLuint bldg_tri_indices_VBO;
  
private:
  int map2Dto1D( int i, int k ) const { return k*width + i; }
  void map1Dto2D( int c, int& i, int& k ) const {i = c%width; k = c/width;}

public:
  // grid setup functions
  Grid(ArgParser* _args);
  void Load();
  void Setup();
  
  // accessors and modifiers
  BoundingBox getBoundingBox() const { return bbox; }
  int getLotStatus(int i,int j,int k) const {
	if ( i >= width || k >= length || i < 0 || k < 0 ) return L_BLOCKED;
	return lots[j][map2Dto1D(i,k)].getStatus(); 
  }
  void setLotStatus(int i,int j,int k,const int& val) { lots[j][map2Dto1D(i,k)].setStatus(val); }
  glm::vec4 getLotColor(int i,int j,int k) const { return lots[j][map2Dto1D(i,k)].getColor(); }
  void setLotColor(int i,int j,int k,const glm::vec4& c) { lots[j][map2Dto1D(i,k)].setColor(c); }
   
  // Grid-filling Functions
  void fill(int i, int k, int bldg_index, int* r_val);
  void fillDriver();
  
  // Rendering Functions
  void initializeVBOs();
  void setupVBOs();
  void drawVBOs();
  void cleanupVBOs();
  
  // Grid-specific Rendering
  void setupGrid();
  void drawGrid();
  
  // Building Rendering
  void setupBuildings();
  void setupBuilding( int index, int i, int k );
  void drawBuildings();
  
};

#endif

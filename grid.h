#include <cassert>
#include <vector>

#include "argparser.h"
#include "boundingbox.h"
#include "vbo_structs.h"
#include "building.h"

/*
class Lot { 
private:
  Block* block;
  int x_index;
  int y_index;
  Building* here;
public:
  // constructor
  Lot(Block* b, int x, int y) 
  : block(b), x_index(x), y_index(y), here(NULL) {};
  
  // accessors
  int getX() const { return x_index; }
  int getY() const { return y_index; }
  Building* getBuilding() const { return here; }
  
  bool isEmpty() { return (here==NULL); }
};

class Block {
private:
  int block_id;
  int width;
  int length;
  std::vector<Lot*> lots;
  Grid* grid;
public:
  Block(int w, int l)
  : width(w), length(l) { block_id = next_block_id++; }
  int getBlockID() const { return block_id; }
};
*/

class Grid {
private:
  ArgParser* args;
  int width = 1;
  int length = 1;
  BoundingBox bbox;
  std::vector<bool> lotStatus;
  //std::vector<Block*> blocks;
  std::vector<Building> bldgs;
  
  // grid geometry information
  std::vector<VBOPosNormalColor> grid_tri_verts;
  std::vector<VBOIndexedTri> grid_tri_indices;
  std::vector<VBOPosNormalColor> grid_edge_verts;
  std::vector<VBOIndexedTri> grid_edge_indices;
  
  // building geometry information
  std::vector<VBOPosNormalColor> bldg_tri_verts;
  std::vector<VBOIndexedTri> bldg_tri_indices;
  
  // VBOs
  GLuint grid_tri_verts_VBO;
  GLuint grid_tri_indices_VBO;
  GLuint grid_edge_verts_VBO;
  GLuint grid_edge_indices_VBO;
  GLuint bldg_tri_verts_VBO;
  GLuint bldg_tri_indices_VBO;
private:
  int map2Dto1D( int i, int k ) const { return k*width + i; }
public:
  // grid setup functions
  Grid(ArgParser* _args);
  void Load();
  void Setup();
  
  // what?
  BoundingBox getBoundingBox() const { return bbox; }
  bool getStatus(int i,int k) const { return lotStatus[map2Dto1D(i,k)]; }
  void setStatus(int i,int k,bool val) { lotStatus[map2Dto1D(i,k)] = val; }
  
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

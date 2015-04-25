#include <cassert>
#include <vector>

#include "argparser.h"
#include "boundingbox.h"
#include "vbo_structs.h"
#include "building.h"

enum LOT_STATUS { L_EMPTY, L_FULL, L_ROAD, L_WATER, L_PARK, L_BLOCKED };

class Lot { 
private:
  int x_index;
  int z_index;
  int status;
  glm::vec4 color;
  Building* here;
public:
  // constructor
  Lot(int x, int z) 
  : x_index(x), z_index(z), here(NULL) { status = 0; setColor(); }
  
  // accessors & modifiers
  int get_x() const { return x_index; }
  int get_z() const { return z_index; }
  glm::vec4 getColor() const { return color; }
  void setColor();
  void setColor( const glm::vec4 & _color ) { color = _color; }
  Building* getBuilding() const { return here; }
  void setBuilding( Building* bldg ) { here = bldg; } 
  
  int getStatus() const { return status; }
  void setStatus( int s ) { status = s; setColor(); }
  bool isEmpty() const { return (here==NULL); }
};

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
  std::vector<Lot> lots;
  
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
  
  // what?
  BoundingBox getBoundingBox() const { return bbox; }
  int getLotStatus(int i,int k) const { return lots[map2Dto1D(i,k)].getStatus(); }
  void setLotStatus(int i,int k,const int& val) { lots[map2Dto1D(i,k)].setStatus(val); }
  glm::vec4 getLotColor(int i,int k) const { return lots[map2Dto1D(i,k)].getColor(); }
  void setLotColor(int i,int k,const glm::vec4& c) { lots[map2Dto1D(i,k)].setColor(c); }
  
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

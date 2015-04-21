#include <cassert>
#include <vector>
#include "argparser.h"
#include "boundingbox.h"
#include "vbo_structs.h"

static int next_block_id = 1;

class Grid;
class Block;
class Lot;
class Building;

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

class Grid {
private:
  ArgParser* args;
  int width = 1;
  int length = 1;
  BoundingBox bbox;
  std::vector<Block*> blocks;
  
  std::vector<VBOPosNormalColor> grid_tri_verts;
  std::vector<VBOIndexedTri> grid_tri_indices;
  std::vector<VBOPosNormalColor> grid_edge_verts;
  std::vector<VBOIndexedTri> grid_edge_indices;
  
  GLuint grid_tri_verts_VBO;
  GLuint grid_tri_indices_VBO;
  GLuint grid_edge_verts_VBO;
  GLuint grid_edge_indices_VBO;
public:
  // grid setup functions
  Grid(ArgParser* _args);
  void Load();
  void Setup();
  
  // what?
  BoundingBox getBoundingBox() const { return bbox; }
  
  // Rendering Functions
  void initializeVBOs();
  void setupVBOs();
  void drawVBOs();
  void cleanupVBOs();
  
  void setupGrid();
  void drawGrid();
  
};

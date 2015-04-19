class Grid;
class Block;
class Lot;
class Building;

class Lot { 
private:
  int x_index;
  int y_index;
  Building* here;
  Block* block;
public:
  // constructor
  Lot(Block* b, int x, int y) 
  : block(b), x_index(x), y_index(y), here(NULL) {};
  
  // accessors
  int getBlockID() const { return block_id; }
  int getX() const { return x_index; }
  int getY() const { return y_index; }
  Building* getBuilding() const { return here; }
  
  bool isEmpty() { return (here==NULL); }
};

class Block {
private:
  static int next_block_id = 1;
  int block_id;
  int width;
  int length;
  std::vector<Lot*> lots;
  Grid* grid;
public:
  Block(int w, int l)
  : width(w), length(l) { block_id = next_block_id++; }
};

class Grid {
private:
  int width;
  int length;
  std::vector<Block*> blocks;
public:
  Grid(int w, int l)
  : width(w), length(l) { }
}

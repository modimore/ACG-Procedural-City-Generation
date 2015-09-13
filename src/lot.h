#ifndef _LOT_H_
#define _LOT_H_

#include <cassert>
#include <vector>

#include "argparser.h"
#include "boundingbox.h"
#include "vbo_structs.h"

enum LOT_STATUS { L_EMPTY, L_FULL, L_ROAD, L_WATER, L_PARK, L_BLOCKED };

class Lot { 
private:
  int x_index;
  int z_index;
  int status;
  glm::vec4 color;
public:
  // constructor
  Lot(int x, int z) 
  : x_index(x), z_index(z){ status = 0; setColor(); }
  
  // accessors & modifiers
  int get_x() const { return x_index; }
  int get_z() const { return z_index; }
  glm::vec4 getColor() const { return color; }
  void setColor();
  void setColor( const glm::vec4 & _color ) { color = _color; }

  int getStatus() const { return status; }
  void setStatus( int s ) { status = s; setColor(); }
};

#endif


#ifndef _BUILDING_H_
#define _BUILDING_H_

#include "boundingbox.h"
#include "lot.h"

class ArgParser;

class Building {
	
friend class Grid;

private:
  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> faces;
  glm::vec4 color;
  //bounding box - based on actual coordinate space
  BoundingBox bbox;
  int width;
  int height;
  int length;
  int size;
  
public:
  Building(ArgParser* args);
  
  int getSize() const {return size;}
  //building attribute shuffling functions
  void sizeShuffle(unsigned int max, unsigned int min);
  void colorShuffle();
  
  //generates 1D packed bounding grid from building obj
  void bGridGen();
};

#endif


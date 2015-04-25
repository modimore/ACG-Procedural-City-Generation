#include "boundingbox.h"

class ArgParser;

class Building {
	
friend class Grid;

private:
  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> faces;
  glm::vec4 color;
  BoundingBox bbox;
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



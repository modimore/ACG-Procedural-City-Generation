#include "grid.h"

#include <iostream>
#include <fstream>

Grid::Grid( ArgParser* _args ) {
  
  args = _args;
  Load();
  bbox.Extend(glm::vec3(width,length,1));
  
}

void Grid::Load() {
  
  std::ifstream istr(std::string(args->path+'/'+args->grid_file));
  
  std::string token;
  while ( istr >> token ) {
	if ( token == "width" ) { istr >> width; }
	else if ( token == "length" ) { istr >> length; }
  }
  
}

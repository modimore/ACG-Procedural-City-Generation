#include "grid.h"

#include <iostream>
#include <fstream>

Grid::Grid( ArgParser* _args ) {
  
  args = _args;
  Load();
  bbox.Extend(glm::vec3(width,1,length));
  
  bldgs.push_back(Building(args));
  
}

void Grid::Load() {
  
  std::ifstream istr(std::string(args->path+'/'+args->grid_file));
  
  std::string token;
  
  istr >> token; assert(token == "width");  istr >> width;
  istr >> token; assert(token == "length"); istr >> length;
  
  lotStatus.resize(width*length,0);
  
  while ( istr >> token ) {
	if (token == "block") {
	  int i,k;
	  istr >> i >> k;
	  setStatus(i,k,1); 
	}
  }
}

void Grid::fill(int bldg_index, int i, int k, int* r_val) {
  
  if ( i == width || k == length ) return;
  if ( getStatus(i,k) ) {
	std::cout << "Lot at (" << i << "," << k << ") is full." << std::endl;
  } 
  else {
	setupBuilding(bldg_index,i,k);
	setStatus(i,k,1);
	*r_val += 1;
  }
  fill(bldg_index,i+1,k,r_val);
  if (i==0)fill(bldg_index,i,k+1,r_val);
  
}

void Grid::fillDriver() {
  int* r_val = new int(0);
  fill(0,0,0,r_val);
  std::cout << *r_val << " buildings created" << std::endl;
  delete r_val;
}

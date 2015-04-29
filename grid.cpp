#include "grid.h"

#include <iostream>
#include <fstream>

void Lot::setColor() {
  if ( status == L_EMPTY || status == L_FULL ) 
    color = glm::vec4(0.5,0.2,0.1,1.0);
  else if ( status == L_ROAD ) 
    color = glm::vec4(0.1,0.1,0.1,1.0);
  else if ( status == L_WATER )
    color = glm::vec4(0.1,0.3,0.7,1.0);
  else if ( status == L_PARK )
    color = glm::vec4(0.3,0.8,0.3,1.0);
  else if ( status == L_BLOCKED )
    color = glm::vec4(0.8,0.1,0.1,1.0);
}

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
  
  int i,k,max_i,max_k;
  
  for ( k = 0; k < length; k++ ) {
	for ( i = 0; i < width; i++ ) {
	  lots.push_back(Lot(i,k));
	}
  }
  
  while ( istr >> token ) {
	if (token == "road") { 
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,_k,L_ROAD);
	    }
	  }
	}
	else if (token == "water") {
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,_k,L_WATER);
	    }
	  }
	}
	else if (token == "park") {
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,_k,L_PARK);
	    }
	  }
	}
	else if (token == "block") {
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,_k,L_BLOCKED);
	    }
	  }
	}
  }
}

void Grid::fill(int bldg_index, int i, int k, int* r_val) {
  
  if ( i == width || k == length ) return;
  if ( getLotStatus(i,k) != L_EMPTY ) {
	std::cout << "Lot status at (" << i << "," << k << ") is " << getLotStatus(i,k) << std::endl;
  }
  else {
	setupBuilding(bldg_index,i,k);
	setLotStatus(i,k,L_FULL);
	lots[map2Dto1D(i,k)].setBuilding(&bldgs[bldg_index]);
	*r_val += 1;
  }
  fill(bldg_index,i+1,k,r_val);
  if (i==0)fill(bldg_index,i,k+1,r_val);
  
}

void Grid::fillDriver() {
  
  if ( length == 0 || width == 0 ) {
	std::cout << "Zero-Volume Space Alert!!" << std::endl;
	return;
  }
  
  int num_placed = 0;
  
  for ( unsigned int bldg_index = 0; bldg_index < bldgs.size(); bldg_index++ ) {
    
    Building bldg = bldgs[bldg_index];
	int k = 0, i = 0;
	int delta = 1;
	
    while ( k < length ) {
	  if ( getLotStatus(i,k) == L_EMPTY ) {
		bool place_building = true;
	    for ( int i_ = 0; i_ < bldg.getWidth(); i_++ ) {
		  for ( int k_ = 0; k_< bldg.getLength(); k_++ ) {
		    if ( (bldg.checkFootprint(i_,k_)) &&
			     (getLotStatus(i+i_,k+k_) != L_EMPTY ) ) {
			  place_building = false;
			  break;
		    }
		  }
	    }
	    if ( place_building ) {
	      setupBuilding(bldg_index,i,k);
	      for ( int i_ = 0; i_ < bldg.getWidth(); i_++ ) {
		    for ( int k_ = 0; k_< bldg.getLength(); k_++ ) {
		      if ( bldg.checkFootprint(i_,k_) )
		        setLotStatus(i+i_,k+k_,L_FULL);
		    }
		  }
		  lots[map2Dto1D(i,k)].setBuilding(&bldgs[bldg_index]);
		  num_placed++;
		}
      }
	  if ( i + delta >= width || i + delta < 0 ) {
	    delta = -delta; k++;
	  } else { i += delta; }
    }
    
  }
  
  std::cout << num_placed << " buildings created" << std::endl;
  
}
/*
void Grid::fillDriver() {
  int* r_val = new int(0);
  fill(0,0,0,r_val);
  std::cout << *r_val << " buildings created" << std::endl;
  delete r_val;
}
*/

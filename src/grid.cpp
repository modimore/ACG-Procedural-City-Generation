#include "grid.h"

#include <iostream>
#include <fstream>


//------------------------------------------------------------------

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

//------------------------------------------------------------------

Grid::Grid( ArgParser* _args ) {
  
  args = _args;
  Load();
  bbox.Extend(glm::vec3(width,1,length));
  
  for (unsigned int ind = 0; ind < args->bldg_files.size(); ind++) {
	for(int i = 0; i < args->num_bldg_alters; ++i){
	  bldgs.push_back(Building(args,ind));
	  int temp_h = bldgs.back().getHeight();
	  if (temp_h > height) { height = temp_h; }
	}
  }
  
  std::vector<Lot> empty_lots;
  for ( int k = 0; k < length; k++ ) {
	for ( int i = 0; i < width; i++ ) {
	  empty_lots.push_back(Lot(i,k));
	}
  }
  for ( int h = 1; h < height; h++ ) {
	lots.push_back( std::vector<Lot>(empty_lots) );
  }
}

void Grid::Load() {
  
  std::ifstream istr(std::string(args->path+'/'+args->grid_file));
  
  std::string token;
  
  istr >> token; assert(token == "width");  istr >> width;
  istr >> token; assert(token == "length"); istr >> length;
  
  int i,k,max_i,max_k;
  
  lots.resize(1);
  for ( k = 0; k < length; k++ ) {
	for ( i = 0; i < width; i++ ) {
	  lots[0].push_back(Lot(i,k));
	}
  }
  
  while ( istr >> token ) {
	if (token == "road") { 
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,0,_k,L_ROAD);
	    }
	  }
	}
	else if (token == "water") {
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,0,_k,L_WATER);
	    }
	  }
	}
	else if (token == "park") {
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,0,_k,L_PARK);
	    }
	  }
	}
	else if (token == "block") {
	  istr >> i >> k >> max_i >> max_k;
	  for ( int _k = k; _k < max_k; _k++ ) {
	    for ( int _i = i; _i < max_i; _i++ ) {
	      setLotStatus(_i,0,_k,L_BLOCKED);
	    }
	  }
	}
  }
}

void Grid::fillDriver() {
  
  if ( length == 0 || width == 0 ) {
	std::cout << "Zero-Volume Space Alert!!" << std::endl;
	return;
  }
  
  int num_placed = 0;
  
  //placeholder for searching through the building list
  unsigned int bldg_index = 0;
  //variables for traversing grid (XZ plane)
  int z = 0; int x = 0;
  //step distance in the x direction
  int delta = 1;
  
  //try to place building in grid
  //if it can be placed, increment and continue
  //if we search through the full grid and cannot fit the building, try the next building
  //end when we have iterated through the whole building list
  while ( (z < length) && (bldg_index < bldgs.size()) ) {
	
	Building bldg = bldgs[bldg_index];

	//CHECK FOR PLACEMENT
	//if lot is empty - preliminary elimination (check corner slot)
	if ( getLotStatus(x,0,z) == L_EMPTY ) {
			
		//consider placing building
		bool place_building = true;
		
		//loop through building footprint (check neighborhood)
		for ( int i = 0; i < bldg.getWidth(); i++ ) {
		  for ( int k = 0; k < bldg.getLength(); k++ ) {
			for ( int j = 0; j < bldg.getHeight(); j++ ) {
			  //check for grid collisions - break out of loop if you cannot place it
			  if ( (bldg.queryVolume(i,j,k)) && (getLotStatus(x+i,j,z+k) != L_EMPTY ) ) {
			    place_building = false;
			    break;
			  }
			}
		  }
		}
		
		//PLACE BUILDING
		if ( place_building ) {
		  setupBuilding(bldg_index,x,z);
		  for ( int i = 0; i < bldg.getWidth(); i++ ) {
			for ( int k = 0; k < bldg.getLength(); k++ ) {
			  for ( int j = 0; j < bldg.getHeight(); j++ ) {
			    if ( bldg.queryVolume(i,j,k) ) {
				  setLotStatus(x+i,j,z+k,L_FULL);
			    }
			  }
			}
		  }
		  num_placed++;
		  bldg_index++;
		}
		 
	}
	
	//move to next valid slot in the grid
	if ( x + delta >= width || x + delta < 0 ) {
		delta = -delta; z++;
	} else { 
		x += delta; 
	}

	if ( z >= length ) {
		bldg_index++;
		z = 0; x = 0; delta = 1;
	}
	
  }
  
  std::cout << num_placed << " buildings created" << std::endl;
  
}

//------------------------------------------------------------------

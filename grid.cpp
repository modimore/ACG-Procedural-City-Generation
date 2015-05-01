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
  
  for (unsigned int ind = 0; ind < args->bldg_files.size(); ind++) {
	std::cout << args->bldg_files[ind] << std::endl;
	for(int i = 0; i < args->num_bldg_alters; ++i){
	  bldgs.push_back(Building(args,ind));
	}
  }
  
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
	bldg.print();
	std::cout << bldg_index << "  ----------" << std::endl;

	//CHECK FOR PLACEMENT
	//if lot is empty - preliminary elimination (check corner slot)
	if ( getLotStatus(x,z) == L_EMPTY ) {
			
		//consider placing building
		bool place_building = true;
		
		//loop through building footprint (check neighborhood)
		for ( int i = 0; i < bldg.getWidth(); i++ ) {
		  for ( int k = 0; k< bldg.getLength(); k++ ) {
			//check for grid collisions - break out of loop if you cannot place it
			if ( (bldg.checkFootprint(i,k)) && (getLotStatus(x+i,z+k) != L_EMPTY ) ) {
			  place_building = false;
			  break;
			}
		  }
		}

		//PLACE BUILDING
		if ( place_building ) {
		  setupBuilding(bldg_index,x,z);
		  for ( int i = 0; i < bldg.getWidth(); i++ ) {
			for ( int k = 0; k< bldg.getLength(); k++ ) {
			  if ( bldg.checkFootprint(i,k) ) {
				setLotStatus(x+i,z+k,L_FULL);
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

	if ( z > length ) {
		bldg_index++;
		z = 0; x = 0; delta = 1;
	}
	
  }
  
  /*for ( unsigned int bldg_index = 0; bldg_index < bldgs.size(); bldg_index++ ) {
    
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
		  //lots[map2Dto1D(i,k)].setBuilding(&bldgs[bldg_index]);
		  num_placed++;
		}
      }
	  if ( i + delta >= width || i + delta < 0 ) {
	    delta = -delta; k++;
	  } else { i += delta; }
    }
    
  }*/
  
  std::cout << num_placed << " buildings created" << std::endl;
  
}


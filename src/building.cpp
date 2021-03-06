#include <iostream>
#include <fstream>

#include "building.h"
#include "argparser.h"
#include "glCanvas.h"

//------------------------------------------------------------------
// mark cells in the footprint which the given edge (line) lies in
void BoundingGrid::mark(const Edge& e) {
  for ( int j = min_y; j < max_y; j++ ) {
    if ( ((e.start.y > j+1) && (e.end.y > j+1)) || ((e.start.y < j) && (e.end.y < j)) ) continue;
    for ( int i = min_x; i < max_x; i++ ) {
	  if ( ((e.start.x >= i+1) && (e.end.x >= i+1)) || ((e.start.x <= i) && (e.end.x <= i)) ) continue;
	  for ( int k = min_z; k < max_z; k++ ) {
	    if ( ((e.start.z >= k+1) && (e.end.z >= k+1)) || ((e.start.z <= k) && (e.end.z <= k)) ) continue;
	    layers[j-min_y][map2Dto1D(i,k)] = true;
	  }
    }
  }
}

// print out the grid and object footprint
void BoundingGrid::print() {
  std::cout << "[" << min_x << " " << min_y << " " << min_z << "]->"
			<< "[" << max_x << " " << max_y << " " << max_z << "]" << std::endl;
  for ( int i = 0; i < (max_x - min_x); i++ ) {
	std::cout << ": ";
	for ( int j = 0; j < (max_y - min_y); j++ ) {
	  for ( int k = 0; k < (max_z - min_z); k++ ) {
	    std::cout << ((queryVolume(i,j,k)) ? "X":"O") << " ";
	  }
	  std::cout << ": ";
    }
	std::cout << std::endl;
  }
}
//------------------------------------------------------------------

// load a building from a restricted .obj-style formatted file
Building::Building( ArgParser* args, unsigned int index ) {
  
  assert(index < args->bldg_files.size());
  // filepath as specified in arg parser
  std::ifstream istr(std::string(args->path+'/'+args->bldg_files[index]));
  // temporary variables used to read from file
  std::string token;
  float x,y,z;
  int a,b,c;
  
  //random default colour
  colorShuffle();
  
  // read in all the lines in the file
  //(load in initial verts)
  while (istr >> token) {
	// if a vertex is being specified
	// add it to the list of vertices
	// and extend or create the bounding grid if necessary
	if ( token == "v" ) {
	  istr >> x >> y >> z;
	  verts.push_back(glm::vec3(x,y,z));
	}
	
	// if a face is being specified
	// add it to the list of faces
	// and also add all of its edges to the building
	if ( token == "f" ) {
	  istr >> a >> b >> c;
	  faces.push_back(glm::vec3(a,b,c));
	}
  }
  
  //shuffle building dimensions for variety
  sizeShuffle(args->bldg_max, args->bldg_min);
  
  for ( unsigned int i = 0; i < faces.size(); i++ ) {
	  
	  a = (int)faces[i].x;
	  b = (int)faces[i].y;
	  c = (int)faces[i].z;
	  edges.push_back( Edge(verts[a-1],verts[b-1]) );
	  edges.push_back( Edge(verts[b-1],verts[c-1]) );
	  edges.push_back( Edge(verts[c-1],verts[a-1]) );
	  
  }
  
  // set building width and height
  width = (int)bgrid.getDiff().x;
  length = (int)bgrid.getDiff().z;
  height = (int)bgrid.getDiff().y;
  
  // generate the footprint of the building
  bgrid.resizeLayers();
  for ( unsigned int i = 0; i < edges.size(); i++ ) {
	bgrid.mark(edges[i]);
  }
  
}

//make randomized size changes to building
void Building::sizeShuffle(unsigned int max, unsigned int min){

	//generate random coordinate value from given min/max building sizes
	int diff = max-min;
	//if(diff == 0)	diff = 1;
	float v1 = (rand()%diff) + min;
	float v2 = (rand()%diff) + min;

	//stretch along x and/or z axis
	for(unsigned int i = 0; i < verts.size(); ++i){
		verts[i] *= glm::vec3(v1, 1, v2);
		//extend bounding grid to altered size
		if (i == 0) { 
		  bgrid = BoundingGrid(verts[0]);
	    } else {
		  bgrid.extend(verts[i]);
	    }
	}
	
}
//make randomized color changes to building
void Building::colorShuffle(){
	float c1 = (rand()%256)/(float)256;
	float c2 = (rand()%256)/(float)256;
	float c3 = (rand()%256)/(float)256;
	float c4 = (rand()%256)/(float)256;

	color = glm::vec4(c1,c2,c3,c4);
}

//------------------------------------------------------------------

/*
 * 
(3D)
ALGORITHM:
For finding all acceptable states of 3D vertext which lies at intersection of 3 planes with normals N = {n1, n2, n3}
Vertex states defined as assignments to the vertex's six adjacent edges
1. find a point p on each face, edge, vertex of input whose faces are parallel to N
2. for all points p:
	for all directions d:
		Determine geometry at p + (epsilon)d
		if p + (epsilon)d is...
		...on a face, determine its normal direction
		...on an edge, determine normal directions of its two faces and if angle between them is reflex
		...in the interior or exterior
*
*/

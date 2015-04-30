#include <iostream>
#include <fstream>

#include "building.h"
#include "argparser.h"
#include "glCanvas.h"

// mark cells in the footprint which the given edge (line) lies in
void BoundingGrid::mark(const Edge& e) {
  for ( int i = min_x; i < max_x; i++ ) {
	if ( ((e.start.x > i+1) && (e.end.x > i+1)) || ((e.start.x < i) && (e.end.x < i)) ) continue;
	for ( int k = min_z; k < max_z; k++ ) {
	  if ( ((e.start.z > k+1) && (e.end.z > k+1)) || ((e.start.z < k) && (e.end.z < k)) ) continue;
	  footprint[map2Dto1D(i,k)] = true;
	}
  }
}

// print out the grid and object footprint
void BoundingGrid::print() {
  for ( int i = 0; i < (max_x - min_x); i++ ) {
	for ( int k = 0; k < (max_z - min_z); k++ ) {
	  std::cout << ((footprint[map2Dto1D(i,k)]) ? "X":"O") << " ";
	}
	std::cout << std::endl;
  }
}

// load a building from a restricted .obj-style formatted file
Building::Building( ArgParser* args ) {
	
  // filepath as specified in arg parser
  std::ifstream istr(std::string(args->path+'/'+args->bldg_file));
  // temporary variables used to read from file
  std::string token;
  float x,y,z;
  int a,b,c;
  
  // default colour
  color = glm::vec4(0.6,0.2,0.8,0.2);
  
  // read in all the lines in the file
  while (istr >> token) {
	// if a vertex is being specified
	// add it to the list of vertices
	// and extend or create the bounding grid if necessary
	if ( token == "v" ) {
	  istr >> x >> y >> z;
	  verts.push_back(glm::vec3(x,y,z));
	  if ( verts.size() == 1 ) {
		bgrid = BoundingGrid(verts[0]);
	  }	else { 
		bgrid.extend(glm::vec3(x,y,z));
	  }
	}
	// if a face is being specified
	// add it to the list of faces
	// and also add all of its edges to the building
	else if ( token == "f" ) {
	  istr >> a >> b >> c;
	  faces.push_back(glm::vec3(a,b,c));
	  edges.push_back( Edge(verts[a],verts[b]) );
	  edges.push_back( Edge(verts[b],verts[c]) );
	  edges.push_back( Edge(verts[c],verts[a]) );
	}
  }
  
  // set building width and height
  width = (int)bgrid.getDiff().x;
  length = (int)bgrid.getDiff().z;
  
  // generate the footprint of the building
  bgrid.resizeFootprint();
  for ( unsigned int i = 0; i < edges.size(); i++ ) {
	bgrid.mark(edges[i]);
  }
  
  std::cout << getSize() << std::endl;
  bgrid.print();
  
}


//make randomized size changes to building
void Building::sizeShuffle(unsigned int max, unsigned int min){

	//generate random coordinate value from given min/max building sizes
	int diff = max-min;
	float v1 = (rand()%diff) + min;
	float v2 = (rand()%diff) + min;

	//stretch along x and/or z axis
	for(unsigned int i = 0; i < verts.size(); ++i){
		verts[i] *= glm::vec3(v1, 1, v2);
		bbox.Extend(verts[i]);
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



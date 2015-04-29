#include <iostream>
#include <fstream>

#include "building.h"
#include "argparser.h"
#include "glCanvas.h"

Building::Building( ArgParser* args ) {
	
  std::ifstream istr(std::string(args->path+'/'+args->bldg_file));
  std::string token;
  float x,y,z;
  int a,b,c;
  
  color = glm::vec4(0.6,0.2,0.8,0.2);
  
  while (istr >> token) {
	if ( token == "v" ) {
	  istr >> x >> y >> z;
	  verts.push_back(glm::vec3(x,y,z));
	  bbox.Extend(glm::vec3(x,y,z));
	}
	else if ( token == "f" ) {
	  istr >> a >> b >> c;
	  faces.push_back(glm::vec3(a,b,c));
	}
  }
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



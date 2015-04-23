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

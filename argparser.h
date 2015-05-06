#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cassert>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "mtrand.h"

#include <iostream>

// ================================================================================
// ================================================================================

inline void separatePathAndFile(const std::string &input, std::string &path, std::string &file) {
  // we need to separate the filename from the path
  // (we assume the vertex & fragment shaders are in the same directory)
  // first, locate the last '/' in the filename
  size_t last = std::string::npos;  
  while (1) {
    int next = input.find('/',last+1);
    if (next != (int)std::string::npos) { 
      last = next;
      continue;
    }
    next = input.find('\\',last+1);
    if (next != (int)std::string::npos) { 
      last = next;
      continue;
    }
    break;
  }
  if (last == std::string::npos) {
    // if there is no directory in the filename
    file = input;
    path = "."; //path = "../ACG-Procedural-City-Generation";
  } else {
    // separate filename & path
    file = input.substr(last+1,input.size()-last-1);
    path = input.substr(0,last);
  }
}

// ====================================================================================

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {
    DefaultValues();
    
    for (int i = 1; i < argc; i++) {
      if (argv[i] == std::string("-grid")) {
        i++; assert (i < argc); 
        separatePathAndFile(argv[i],path,grid_file);
      } else if (argv[i] == std::string("-bldg") || argv[i] == std::string("-building") ) {
		i++; assert (i<argc);
		bldg_files.clear();
		bldg_files.push_back(std::string(argv[i]));
      } else if (argv[i] == std::string("-bldgs") || argv[i] == std::string("-buildings") ) {
		i++; assert (i<argc);
		int j_max = atoi(argv[i]); 
		assert(i+j_max<argc);
		bldg_files.clear();
		for ( int j = 0; j < j_max; j++ ) {
		  i++; bldg_files.push_back(std::string(argv[i]));
		}
      } else if (argv[i] == std::string("-size")) {
        i++; assert (i < argc); 
	    width = height = atoi(argv[i]);
      } else if ( argv[i] == std::string("-bldg_max") ) {
		i++; assert(i<argc);
		bldg_max = atoi(argv[i]);
      } else if ( argv[i] == std::string("-bldg_min") ) {
		i++; assert(i<argc);
		bldg_min = atoi(argv[i]);
	  } else if ( argv[i] == std::string("-bldg_alters") || argv[i] == std::string("-bc") ) {
		i++; assert(i<argc);
		num_bldg_alters = atoi(argv[i]);
	  } else {
	    printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
	    assert(0);
      }
    }
  }

  // ===================================
  // ===================================

  void DefaultValues() {
    width = 500;
    height = 500;
    
    bldg_max = 3;
    bldg_min = 1;
    num_bldg_alters = 7;
    
    path = "../ACG-Procedural-City-Generation";
    grid_file = "simple_grid.txt";
    bldg_files.push_back("box.obj");
    
    bounding_box = false;
    // uncomment for deterministic randomness
    // mtrand = MTRand(37);
    
  }

  // ===================================
  // ===================================
  // REPRESENTATION
  // all public! (no accessors)

  std::string path;
  std::string grid_file;
  std::vector< std::string > bldg_files;
  int width;
  int height;
  
  int bldg_min;
  int bldg_max;
  int num_bldg_alters;

  bool bounding_box;

  // default initialization
  MTRand mtrand;

};

// ================================================================================

#endif

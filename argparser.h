#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cassert>
#include <string>
#include <glm/glm.hpp>
#include "mtrand.h"

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
    path = ".";
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
      if (argv[i] == std::string("-input")) {
        i++; assert (i < argc); 
        separatePathAndFile(argv[i],path,grid_file);
      } else if (argv[i] == std::string("-size")) {
        i++; assert (i < argc); 
	    width = height = atoi(argv[i]);
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
    
    bounding_box = false;
    // uncomment for deterministic randomness
    // mtrand = MTRand(37);
    
  }

  // ===================================
  // ===================================
  // REPRESENTATION
  // all public! (no accessors)

  std::string grid_file;
  std::string path;
  int width;
  int height;

  bool bounding_box;

  // default initialization
  MTRand mtrand;

};

// ================================================================================

#endif

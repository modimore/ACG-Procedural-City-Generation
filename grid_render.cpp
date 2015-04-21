#include "glCanvas.h"

#include "grid.h"
#include "utils.h"

glm::vec4 grid_color(0.2,1.0,0.2,1.0);
glm::vec4 grid_edge_color(1.0,0.0,0.0,1.0);
float grid_edge_thickness = 0.005;

glm::vec3 y_bar(0.0,1.0,0.0);

void Grid::initializeVBOs() {
  glGenBuffers(1,&grid_tri_verts_VBO);
  glGenBuffers(1,&grid_tri_indices_VBO);
  glGenBuffers(1,&grid_edge_verts_VBO);
  glGenBuffers(1,&grid_edge_indices_VBO);
  bbox.initializeVBOs();
}

void Grid::setupVBOs() {
  setupGrid();
}

void Grid::drawVBOs() {
  drawGrid();
}

void Grid::cleanupVBOs() {
  glDeleteBuffers(1,&grid_tri_verts_VBO);
  glDeleteBuffers(1,&grid_tri_indices_VBO);
  glDeleteBuffers(1,&grid_edge_verts_VBO);
  glDeleteBuffers(1,&grid_edge_indices_VBO);
  bbox.cleanupVBOs();
}

void Grid::setupGrid() {
  
  int i,k;
  
  for ( k = 0; k <= length; k++ ) {
	for ( i = 0; i <= width; i++ ) {
	  glm::vec3 pos(float(i),0.0,float(k));
	  grid_tri_verts.push_back( VBOPosNormalColor(pos,y_bar,grid_color) );
	}
  }
  
  for ( k = 0; k < length; k++ ) {
	for ( i = 0; i < width; i++ ) {
	  glm::vec3 pos(float(i),0.0,float(k));
	  addEdgeGeometry( grid_edge_verts, grid_edge_indices,
	                   pos, pos + glm::vec3(1.0,0.0,0.0),
	                   grid_edge_color, grid_edge_color,
	                   grid_edge_thickness, grid_edge_thickness );
	  addEdgeGeometry( grid_edge_verts, grid_edge_indices,
	                   pos, pos + glm::vec3(0.0,0.0,1.0),
	                   grid_edge_color, grid_edge_color,
	                   grid_edge_thickness, grid_edge_thickness );
	  addEdgeGeometry( grid_edge_verts, grid_edge_indices,
	                   pos + glm::vec3(1.0,0.0,0.0), pos + glm::vec3(1.0,0.0,1.0),
	                   grid_edge_color, grid_edge_color,
	                   grid_edge_thickness, grid_edge_thickness );
	  addEdgeGeometry( grid_edge_verts, grid_edge_indices,
	                   pos + glm::vec3(0.0,0.0,1.0), pos + glm::vec3(1.0,0.0,1.0),
	                   grid_edge_color, grid_edge_color,
	                   grid_edge_thickness, grid_edge_thickness );
	}
  }
  
  for ( k = 0; k < length*width; k += width+1 ) {
    for ( i = 0; i < width; i++ ) {
	  grid_tri_indices.push_back( VBOIndexedTri(k+i,k+width+i+1,k+i+1) );
	  grid_tri_indices.push_back( VBOIndexedTri(k+i+1,k+width+i+1,k+width+i+2) );
	}
  }
  
  glBindBuffer(GL_ARRAY_BUFFER,grid_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*grid_tri_verts.size(),&grid_tri_verts[0],GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,grid_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri)*grid_tri_indices.size(),&grid_tri_indices[0],GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,grid_edge_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*grid_edge_verts.size(),&grid_edge_verts[0],GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,grid_edge_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri)*grid_edge_indices.size(),&grid_edge_indices[0],GL_STATIC_DRAW);  
}

void Grid::drawGrid() {
  HandleGLError("enter draw grid");
  
  glBindBuffer(GL_ARRAY_BUFFER,grid_tri_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,grid_tri_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, grid_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  
  glBindBuffer(GL_ARRAY_BUFFER,grid_edge_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,grid_edge_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, grid_edge_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  
  HandleGLError("leaving draw grid");
}

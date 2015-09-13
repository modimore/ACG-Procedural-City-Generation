#include "glCanvas.h"

#include <iostream>
#include <fstream>

#include "grid.h"
#include "utils.h"

glm::vec4 grid_color(0.5,0.5,0.5,0.5);
glm::vec4 grid_edge_color(0.1,0.1,0.1,1.0);
float grid_edge_thickness = 0.005;

glm::vec3 y_bar(0.0,1.0,0.0);

void Grid::initializeVBOs() {
  // Grid VBOs
  glGenBuffers(1,&lot_tri_verts_VBO);
  glGenBuffers(1,&lot_tri_indices_VBO);
  glGenBuffers(1,&grid_edge_verts_VBO);
  glGenBuffers(1,&grid_edge_indices_VBO);
  // Building VBOs
  glGenBuffers(1,&bldg_tri_verts_VBO);
  glGenBuffers(1,&bldg_tri_indices_VBO);
  // Bounding Box VBOs
  bbox.initializeVBOs();
}

void Grid::setupVBOs() {
  setupGrid();
  setupBuildings();
}

void Grid::drawVBOs() {
  drawGrid();
  drawBuildings();
}

void Grid::cleanupVBOs() {
  // Grid VBOs
  glDeleteBuffers(1,&lot_tri_verts_VBO);
  glDeleteBuffers(1,&lot_tri_indices_VBO);
  glDeleteBuffers(1,&grid_edge_verts_VBO);
  glDeleteBuffers(1,&grid_edge_indices_VBO);
  // Building VBOs
  glDeleteBuffers(1,&bldg_tri_verts_VBO);
  glDeleteBuffers(1,&bldg_tri_indices_VBO);
  bbox.cleanupVBOs();
}

void Grid::setupGrid() {
  
  int i,k;
  
  for ( k = 0; k < length; k++ ) {
	for ( i = 0; i < width; i++ ) {
	  int start = lot_tri_verts.size();
	  glm::vec3 pos(float(i),0.0,float(k));
	  lot_tri_verts.push_back(
	    VBOPosNormalColor(pos,y_bar,getLotColor(i,0,k)));
	  lot_tri_verts.push_back(
	    VBOPosNormalColor(pos+glm::vec3(0.0,0.0,1.0),y_bar,getLotColor(i,0,k)));
	  lot_tri_verts.push_back(
	    VBOPosNormalColor(pos+glm::vec3(1.0,0.0,1.0),y_bar,getLotColor(i,0,k)));
	  lot_tri_verts.push_back(
	    VBOPosNormalColor(pos+glm::vec3(1.0,0.0,0.0),y_bar,getLotColor(i,0,k)));
	  lot_tri_indices.push_back(VBOIndexedTri(start,start+1,start+2));
	  lot_tri_indices.push_back(VBOIndexedTri(start,start+2,start+3));
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
  
  glBindBuffer(GL_ARRAY_BUFFER,lot_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*lot_tri_verts.size(),&lot_tri_verts[0],GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,lot_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri)*lot_tri_indices.size(),&lot_tri_indices[0],GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,grid_edge_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*grid_edge_verts.size(),&grid_edge_verts[0],GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,grid_edge_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri)*grid_edge_indices.size(),&grid_edge_indices[0],GL_STATIC_DRAW);  
}

void Grid::drawGrid() {
  HandleGLError("enter draw grid");
  
  glBindBuffer(GL_ARRAY_BUFFER,lot_tri_verts_VBO); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,lot_tri_indices_VBO); 
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3) );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, lot_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
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
  glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor), (void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, grid_edge_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  
  HandleGLError("leaving draw grid");
}

void Grid::setupBuildings() {
  
  glBindBuffer(GL_ARRAY_BUFFER,bldg_tri_verts_VBO); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VBOPosNormalColor)*bldg_tri_verts.size(),&bldg_tri_verts[0],GL_STATIC_DRAW); 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bldg_tri_indices_VBO); 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(VBOIndexedTri)*bldg_tri_indices.size(),&bldg_tri_indices[0],GL_STATIC_DRAW); 
  
}


//generatebuildings will feed into this function
void Grid::setupBuilding(int index, int i, int k) {
  
  Building bldg = bldgs[index];
  bldg.colorShuffle();
 
  for ( unsigned int j = 0; j < bldg.faces.size(); j++ ) {
	glm::vec3 f = bldg.faces[j];
	glm::vec3 normal = ComputeNormal(bldg.verts[(int)f.x-1],bldg.verts[(int)f.y-1],bldg.verts[(int)f.z-1]);
	int start = bldg_tri_verts.size();
	glm::vec3 p1 = bldg.verts[(int)f.x-1] + glm::vec3(float(i),0.0,float(k));
	glm::vec3 p2 = bldg.verts[(int)f.y-1] + glm::vec3(float(i),0.0,float(k));
	glm::vec3 p3 = bldg.verts[(int)f.z-1] + glm::vec3(float(i),0.0,float(k));
	bldg_tri_verts.push_back(VBOPosNormalColor(p1,normal,bldg.color));
	bldg_tri_verts.push_back(VBOPosNormalColor(p2,normal,bldg.color));
	bldg_tri_verts.push_back(VBOPosNormalColor(p3,normal,bldg.color));
	bldg_tri_indices.push_back(VBOIndexedTri(start,start+1,start+2));
  }
  
}

void Grid::drawBuildings() {
  HandleGLError("enter draw buildings");
  
  glBindBuffer(GL_ARRAY_BUFFER,bldg_tri_verts_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bldg_tri_indices_VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)sizeof(glm::vec3));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,sizeof(VBOPosNormalColor),(void*)(sizeof(glm::vec3)*2));
  glDrawElements(GL_TRIANGLES, bldg_tri_indices.size()*3,GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  
  HandleGLError("leaving draw buildings");
}

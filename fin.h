#ifndef FIN_H
#define FIN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "common.h"

using namespace std;

typedef struct _mesh{
  glm::vec3 attribute; // {vertices, faces, edges}
  vector<glm::vec3> vertices;
  struct {
  	vector<glm::vec3> colour;
  	vector<glm::vec3> normal; // in vbos  
  							  // normal of a triangle mesh n = (p1 - p0) * (p2-p0) // assuming counter-clock-wise
  };
  struct texture {
  	vector<glm::vec3> diffuse;
  	vector<glm::vec3> ambient;
  	vector<glm::vec3> specular;
  	GLfloat shineness;
  };
  //vector<glm::vec3> vertices;
  vector<int> vec_in_face;
  vector<glm::vec4> faces;
  vector<GLuint> indices; // for drawing?
  //vector<glm::vec3> normals; // (DEPRECATED) using the same normal for the same face for lighting? [Flat Shading approach]
  							 // (Shader-based: Has to compute normal PER VERTEX- so no saving whatsoever)
  							 // 

  /* 
  	(Gouraund Shading: NEED TO EASILY FIND ALL ITS ADJACENT FACES 
  					    - AVERAGE NORMALISED NORMALS OF ADJACENT FACES)  
  	1. Compute Vertex Color and Shade using vertex shader
  	2. Computation done in Vertex Shader
  	3. The rest of the face will come out automatically (interpolation done by the shader) // fColor = color
  */

  /* 
  	Phong Shading 
  	1. Interpolate normals at every vertices, edges, and interior point
  	2. Has to do it in fragment shader
  */

  // Texture Lighting: if no change intended, can be set in shader
  // Can compute normal/angles in GLSL

  /* 
	 Light Source
	 1. position
	 2. direction
	 3. intensity
  */
} MESH;

typedef struct _light{
 glm::vec4 diffuse0 = glm::vec4(1.0, 1.0, 1.0, 1.0);
 glm::vec4 ambient0 = glm::vec4(1.0, 1.0, 1.0, 1.0);
 glm::vec4 specular0 = glm::vec4(1.0, 1.0, 1.0, 1.0);
 union { // either positional or directional
 	glm::vec4 light0_pos;// = glm::vec4(10, 20 ,30, 1.0); // point
 	glm::vec4 light0_dir;// = glm::vec4(10, 20 ,30, 0.0); // direction
 };
 glm::vec4 dropoff_coeff = glm::vec4(0.1, 0.1, 0.1, 0.1); //a, b, c, d
} LIGHT;


/* Bling-Phong model
   halfway vector: h = (l+v)/ |l+v|  
   1. substitute r*v with n*h, in order to avoid calculating r (perfect reflection) 
   2. weaken specular -> compensate with a smaller alpha  */

void read_mesh(string filename, MESH& mesh);
void read_all_meshes(vector<string>& filenames, vector<MESH>& all_meshes);
void print_mesh_info(MESH& mesh);

#endif

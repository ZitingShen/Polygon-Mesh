#ifndef FIN_H
#define FIN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include "common.h"
#include "glm/gtx/transform.hpp"

using namespace std;

typedef enum _p_mode{PARALLEL, PERSPECTIVE} p_mode;
typedef enum _d_mode{POINT, EDGE, FACE} d_mode;

  /* 
   Light Source
   1. position
   2. direction
   3. intensity
  */
typedef struct _light{
 // using light
 glm::vec4 diffuse0 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
 glm::vec4 ambient0 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
 glm::vec4 specular0 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
 glm::vec4 light0;
 glm::vec4 dropoff_coeff = glm::vec4(0.1f, 0.1f, 0.1f, 0.1f); //a, b, c, d
} LIGHT;

struct TEXTURE{
  glm::vec3 diffuse;
  glm::vec3 ambient;
  glm::vec3 specular;
  GLfloat shineness;
};

struct VERTEX{
  glm::vec3 pos;
  glm::vec3 normal;
};

struct FACES{
  vector<GLushort> num_v;
  vector<GLushort> edge_indices;  // origianl indices verbatim
  vector<GLushort> draw_indices; // triangulised indices
  vector<glm::vec3> normal;     // nromals of triangulaised faces
};

class MESH {
  public:
    /* public data member */
    GLuint num_v;
    GLuint num_f; // not necessarily the num of triangles to be drawn
    GLuint num_e;
    vector<VERTEX> vertices;       // vertex pos and vertex normal
    vector<VERTEX> vertices_flat;  // vertex pos and face normal
    vector<glm::mat4> transforms;  // transform matrices for each copy of the objects
    TEXTURE  texture;
    FACES     faces;
    glm::vec4 ambient_product, diffuse_product, specular_product;
    /* Constructor */
    MESH();
    void setup(GLuint shader);
    void bind_flat(GLuint shader);
    void bind_other(GLuint shader);
    void compute_face_normal();
    void compute_vertex_normal();
    void compute_light_product(LIGHT& THE_LIGHT);
    void draw(GLuint shader, glm::mat4& PROJ_MAT, glm::mat4& MV_MAT, 
      LIGHT& THE_LIGHT, d_mode DRAW_MODE);
    void rotate();
  private:
    GLuint vao, vbo_flat, vbo_other, ebo_edge, ebo_other;
};

 // in vbos
 // normal of a triangle mesh n = (p1 - p0) * (p2-p0) // assuming counter-clock-wise
 //vector<glm::vec3> vertices;
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


/* Bling-Phong model
   halfway vector: h = (l+v)/ |l+v|
   1. substitute r*v with n*h, in order to avoid calculating r (perfect reflection) 
   2. weaken specular -> compensate with a smaller alpha  */

void read_mesh(string filename, MESH& mesh, int count,
               glm::vec3& max_xyz, glm::vec3& min_xyz,
               GLuint shader);
void read_all_meshes(map<string, int>& filenames, vector<MESH>& all_meshes,
                     glm::vec3& max_xyz, glm::vec3& min_xyz,
                     GLuint shader);
void print_mesh_info(MESH& mesh);
void load_texture(MESH& mesh, const GLfloat* texture);
void load_random_texture(vector<MESH>& all_meshes);
#endif

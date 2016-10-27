#include "fin.h"

MESH::MESH(){
  this->num_v = 0;
  this->num_f = 0;
  this->num_e = 0;
}

MESH::MESH(GLuint num_v,
           GLuint num_f,
           GLuint num_e,
           VERTICES vertices,
           TEXTURE  texture,
           FACE     faces){
  this->num_v = num_v;
  this->num_f = num_f;
  this->num_e = num_e;
  this->vertices = vertices;
  this->texture  = texture;
  this->faces    = faces;

}

void MESH::compute_face_normal(){
  glm::vec3 normal;
  unsigned int count = 0;
  // Assuming all vertices are given in counter-clock order
  for (int i=0; i<(int)this->num_f; i++){
    normal = glm::cross(this->vertices.pos[ this->faces.indices[count + 2] ] 
                      - this->vertices.pos[ this->faces.indices[count + 1] ],
                        this->vertices.pos[ this->faces.indices[count] ] 
                      - this->vertices.pos[ this->faces.indices[count + 1]] );
    this->faces.normal.push_back(glm::normalize(normal));
    count += (int)this->faces.num_v[i]; // moving to the next face
  }
}

void MESH::compute_vertex_normal(){
  vector<vector<int> > faces_per_vertex(this->num_v, vector<int>());
  glm::vec3 normal(0, 0, 0);
  int count = 0;
  for (int i=0; i<(int)this->num_f; i++){  // which face we are looking at
    for (int j=0; j<(int)this->faces.num_v[i]; j++){  // all vertices in this face
      //cout << this->faces.num_v[i] << endl;
      faces_per_vertex[ this->faces.indices[count] ].push_back(i);
      count++;      
    }
  }    
//    for (auto k = faces_per_vertex[2].begin(); k != faces_per_vertex[2].end(); k++)
//     cout << (*k) << ' ';
//    cout << endl;

  for (int i=0; i<(int)this->num_v; i++){
    for (int j=0; j<(int)faces_per_vertex[i].size(); j++){
      normal += this->faces.normal[faces_per_vertex[i][j]];
    }
    normal = glm::normalize(normal * (1.0f / (float)faces_per_vertex[i].size()));
    this->vertices.normal.push_back(normal);
    normal = glm::vec3(0, 0, 0);
//    cout << this->vertices.normal[i][0] << " "
//         << this->vertices.normal[i][1] << " "
//         << this->vertices.normal[i][2] << " " << endl;
  }
}

void MESH::interleave(GLfloat arry[]){
  int count = 0;
  for (int i=0; i<this->num_v; i++){
    arry[i] = this->vertices.pos[count][0];
    arry[i+1] = this->vertices.pos[count][1];
    arry[i+2] = this->vertices.pos[count][2];
    arry[i+3] = this->vertices.normal[count][0];
    arry[i+4] = this->vertices.normal[count][1];
    arry[i+5] = this->vertices.normal[count][2];
    count += 6;
  }
}

void read_mesh(string filename, MESH& mesh,
               glm::vec3& max_xyz,
               glm::vec3& min_xyz){
  glm::vec3 holder;
  glm::vec4 holder_fac;
  string off;
  int vec_in_fac;
  int temp;
  ifstream my_fin;
  my_fin.open(filename);
  if (!my_fin.is_open()){
    cerr << "READ_MESH: CAN NOT OPEN FILE: " << filename <<endl;
    return;
  }
  getline(my_fin, off);
  if (off.compare("OFF") != 0){
    cerr << "READ_MESH: NOT AN OFF FILE: " << filename <<endl;
    return;
  }
  /* reading attributs */
  my_fin >> mesh.num_v;
  my_fin >> mesh.num_f;
  my_fin >> mesh.num_e;
  /* reading vertices */
  for (int i=0; i<(int)mesh.num_v; i++){
    for (int j=0; j<3; j++){
      my_fin >> holder[j];
    }
    max_xyz[0] = max_xyz[0]<holder[0]?holder[0]:max_xyz[0];
    max_xyz[1] = max_xyz[1]<holder[1]?holder[1]:max_xyz[1];
    max_xyz[2] = max_xyz[2]<holder[2]?holder[2]:max_xyz[2];
    min_xyz[0] = min_xyz[0]>holder[0]?holder[0]:min_xyz[0];
    min_xyz[1] = min_xyz[1]>holder[1]?holder[1]:min_xyz[1];
    min_xyz[2] = min_xyz[2]>holder[2]?holder[2]:min_xyz[2];
    mesh.vertices.pos.push_back(holder);
  }
  /* reading faces */
  for (int i=0; i<(int)mesh.num_f; i++){
    my_fin >> vec_in_fac;
    mesh.faces.num_v.push_back(static_cast<GLubyte>(vec_in_fac));
    for (int j=0; j<vec_in_fac;j++){
      my_fin >> temp;
      mesh.faces.indices.push_back(temp);
    }
  }
  my_fin.close();
  mesh.compute_face_normal();
  mesh.compute_vertex_normal();
}


void read_all_meshes(vector<string>& filenames, vector<MESH>& all_meshes,
                     glm::vec3& max_xyz,
                     glm::vec3& min_xyz){
  max_xyz[0] = -FLT_MAX;
  max_xyz[1] = -FLT_MAX;
  max_xyz[2] = -FLT_MAX;
  min_xyz[0] = FLT_MAX;
  min_xyz[1] = FLT_MAX;
  min_xyz[2] = FLT_MAX;
  MESH a_mesh;
  for (int i=0; i<(int)filenames.size(); i++){
    read_mesh(filenames[i], a_mesh, max_xyz, min_xyz);
    all_meshes.push_back(a_mesh);
  }
}

void print_mesh_info(MESH& mesh){
  int count = 0;
  cout << mesh.num_v << " ";
  cout << mesh.num_f << " ";
  cout << mesh.num_e << " " << endl;
  for (int i = 0; i<(int)mesh.num_v; i++){
    for (int j=0; j<3; j++){
      cout << mesh.vertices.pos[i][j] << " ";
    }
    cout << endl;
  }

  for (int i = 0; i<(int)mesh.num_f; i++){
    cout << (int)mesh.faces.num_v[i] << " ";
    for (int j=0; j<(int)mesh.faces.num_v[i]; j++){
      cout << (int)mesh.faces.indices[count] << " ";
      count++;
    }
    cout << endl;
  }
}

void load_texture(MESH& mesh, const GLfloat* texture){
  mesh.texture.diffuse[0] = texture[0];
  mesh.texture.diffuse[1] = texture[1];
  mesh.texture.diffuse[2] = texture[2];
  mesh.texture.ambient[0] = texture[3];
  mesh.texture.ambient[1] = texture[4];
  mesh.texture.ambient[2] = texture[5];
  mesh.texture.specular[0] = texture[6];
  mesh.texture.specular[1] = texture[7];
  mesh.texture.specular[2] = texture[8];
  mesh.texture.shineness   = texture[9];
}

void load_random_texture(vector<MESH>& all_meshes){
  for (int i=0; i<(int)all_meshes.size(); i++){
    load_texture(all_meshes[i], material_props[MAT_DEFAULT]);
  }
}
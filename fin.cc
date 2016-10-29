#include "fin.h"

MESH::MESH(){
  this->num_v = 0;
  this->num_f = 0;
  this->num_e = 0;
}

void MESH::bind(){
  // bind vao
  cout << this->vao << endl;
    cout << "got here " << endl;
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  // bind vbo
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               this->vertices.size()*sizeof(VERTEX),
               &this->vertices[0], GL_STATIC_DRAW);

  //bind ebo
  glGenBuffers(1, &this->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               this->faces.draw_indices.size()*sizeof(GLuint),
               &this->faces.draw_indices[0],
               GL_STATIC_DRAW);

  // Vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT,
                        GL_FALSE,
                        sizeof(VERTEX),
                        (GLvoid*)0);
  // Vertex Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT,
                        GL_FALSE,
                        sizeof(VERTEX),
                        (GLvoid*)offsetof(VERTEX, normal));

  glBindVertexArray(0);
}

void MESH::bind_flat(){
  // bind vbo
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               this->vertices_flat.size()*sizeof(VERTEX),
               &this->vertices_flat[0], GL_STATIC_DRAW);
}

void MESH::bind_other(){
  // bind vbo
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               this->vertices.size()*sizeof(VERTEX),
               &this->vertices[0], GL_STATIC_DRAW);
}

void MESH::get_render_data(GLuint& vao, GLuint&vbo, GLuint& ebo){
  vao = this->vao;
  vbo = this->vbo;
  ebo = this->ebo;
}

void MESH::compute_face_normal(){
  glm::vec3 normal;
  int num_triangles = this->faces.draw_indices.size() / 3;
  unsigned int count = 0;
  // Assuming all vertices are given in counter-clock order
  for (int i=0; i<num_triangles; i++){
    normal = glm::cross(this->vertices[ this->faces.draw_indices[count + 2] ].pos
                      - this->vertices[ this->faces.draw_indices[count + 1] ].pos,
                        this->vertices[ this->faces.draw_indices[count    ] ].pos
                      - this->vertices[ this->faces.draw_indices[count + 1] ].pos );
    this->faces.normal.push_back(glm::normalize(normal));
    count += 3; // moving to the next face
  }
}

void MESH::compute_vertex_normal(){
  vector<vector<int> > faces_per_vertex(this->num_v, vector<int>());
  glm::vec3 normal(0, 0, 0);
  int num_triangles = this->faces.draw_indices.size() / 3;
  int count = 0;
  for (int i=0; i<num_triangles; i++){  // which face we are looking at
    for (int j=0; j<3; j++){  // all vertices in this face
      //cout << this->faces.num_v[i] << endl;
      faces_per_vertex[ this->faces.draw_indices[count] ].push_back(i);
      count++;
    }
  }
//    for (auto k = faces_per_vertex[2].begin(); k != faces_per_vertex[2].end(); k++)
//    cout << (*k) << ' ';
//    cout << endl;
  count = 0;
  for (int i=0; i<(int)this->num_v; i++){
    for (int j=0; j<(int)faces_per_vertex[i].size(); j++){
      normal += this->faces.normal[faces_per_vertex[i][j]];
    }
    normal = glm::normalize(normal * (1.0f / (float)faces_per_vertex[i].size()));
    this->vertices[i].normal[0] = normal[0];
    this->vertices[i].normal[1] = normal[1];
    this->vertices[i].normal[2] = normal[2];
    normal = glm::vec3(0, 0, 0);
//    cout << this->vertices.normal[i][0] << " "
//         << this->vertices.normal[i][1] << " "
//         << this->vertices.normal[i][2] << " " << endl;
  }
  this->vertices_flat.reserve(this->faces.normal.size()*3);
  this->vertices_flat.resize(this->faces.normal.size()*3);
  for (int i=0; i<(int)this->faces.normal.size(); i++){
    for (int j=0; j<3; j++){
      //cout << i*3+j << " " << this->faces.draw_indices[i*3+j] << endl;
      this->vertices_flat[i*3+j].pos = this->vertices[ this->faces.draw_indices[i*3+j] ].pos;
      this->vertices_flat[i*3+j].normal = this->vertices[ this->faces.draw_indices[i*3+j] ].normal;
    }
  }
}

void read_mesh(string filename, MESH& mesh,
               glm::vec3& max_xyz,
               glm::vec3& min_xyz){
  glm::vec3 holder;
  vector<GLubyte> holder_indices;
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
  mesh.vertices.reserve(mesh.num_v);
  mesh.vertices.resize(mesh.num_v);
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
    mesh.vertices[i].pos[0] = holder[0];
    mesh.vertices[i].pos[1] = holder[1];
    mesh.vertices[i].pos[2] = holder[2];
  }
  /* reading faces */
  for (int i=0; i<(int)mesh.num_f; i++){
    my_fin >> vec_in_fac;
    mesh.faces.num_v.push_back(static_cast<GLubyte>(vec_in_fac));
    if (vec_in_fac == 3){
      for (int j=0; j<vec_in_fac;j++){
        my_fin >> temp;
        mesh.faces.indices.push_back(temp);
        mesh.faces.draw_indices.push_back(temp);
      }
    }else{ // when more than three vertices in a face
      for (int j=0; j<vec_in_fac;j++){
        my_fin >> temp;
        mesh.faces.indices.push_back(temp);
        holder_indices.push_back(temp);
      }
      for (int j=0; j<(vec_in_fac-2);j++){
        mesh.faces.draw_indices.push_back(holder_indices[0]);
        mesh.faces.draw_indices.push_back(holder_indices[j+1]);
        mesh.faces.draw_indices.push_back(holder_indices[j+2]);
      }
      holder_indices.clear(); // reset holder
    }
  }
  my_fin.close();
  mesh.compute_face_normal();
  mesh.compute_vertex_normal();

  mesh.bind();
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
  for (int i=0; i<(int)filenames.size(); i++){
    //cout << "reading " << filenames[i] << endl;
    read_mesh(filenames[i], all_meshes[i], max_xyz, min_xyz);
  }
}

void print_mesh_info(MESH& mesh){
  int count = 0;
  cout << mesh.num_v << " ";
  cout << mesh.num_f << " ";
  cout << mesh.num_e << " " << endl;
  /* print vertcies */
  for (int i = 0; i<(int)mesh.num_v; i++){
    for (int j=0; j<3; j++){
      cout << mesh.vertices[i].pos[j] << " ";
    }
    cout << endl;
  }
  /* print faces */
  for (int i = 0; i<(int)mesh.num_f; i++){
    cout << (int)mesh.faces.num_v[i] << " ";
    for (int j=0; j<(int)mesh.faces.num_v[i]; j++){
      cout << (int)mesh.faces.indices[count] << " ";
      count++;
    }
    cout << endl;
  }
  /* print draw faces */
  cout << (int)mesh.faces.draw_indices.size() / 3<< endl;
  for (int i=0; i <(int)mesh.faces.draw_indices.size(); i++){
    cout << (int)mesh.faces.draw_indices[i] << " ";
    if ((i+1) % 3 == 0)
      cout << endl;
  }
  /* print face normals */
  cout << (int)mesh.faces.normal.size()<< endl;
  for (int i=0; i <(int)mesh.faces.normal.size(); i++){
    cout << mesh.faces.normal[i][0] << " ";
    cout << mesh.faces.normal[i][1] << " ";
    cout << mesh.faces.normal[i][2] << " "  << endl;
  }
  /* print vertex normals */
  cout << (int)mesh.vertices.size()<< endl;
  for (int i=0; i <(int)mesh.vertices.size(); i++){
    cout << mesh.vertices[i].normal[0] << " ";
    cout << mesh.vertices[i].normal[2] << " ";
    cout << mesh.vertices[i].normal[3] << " "  << endl;
  }
  cout << "got here" <<endl;
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

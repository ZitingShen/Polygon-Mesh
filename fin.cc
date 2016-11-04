#include "fin.h"

static GLuint make_bo(GLenum type, const void *buf, GLsizei buf_size) {
  GLuint bufnum;
  glGenBuffers(1, &bufnum);
  glBindBuffer(type, bufnum);
  glBufferData(type, buf_size, buf, GL_STATIC_DRAW);
  return bufnum;
}

MESH::MESH(){
  this->num_v = 0;
  this->num_f = 0;
  this->num_e = 0;
}

void MESH::setup(GLuint shader){
  // bind vao
  glGenVertexArrays(1, &this->vao_other);
  glGenVertexArrays(1, &this->vao_flat);
  // generate vbo
  this->vbo_other = make_bo(GL_ARRAY_BUFFER, &this->vertices[0], 
    this->vertices.size()*sizeof(VERTEX));
  this->vbo_flat = make_bo(GL_ARRAY_BUFFER, &this->vertices_flat[0], 
    this->vertices_flat.size()*sizeof(VERTEX));
  //generate ebo
  this->ebo_other = make_bo(GL_ELEMENT_ARRAY_BUFFER, &this->faces.draw_indices[0],
    this->faces.draw_indices.size()*sizeof(GLuint));
  this->ebo_edge = make_bo(GL_ELEMENT_ARRAY_BUFFER, &this->faces.edge_indices[0],
    this->faces.edge_indices.size()*sizeof(GLuint));
  this->ebo_flat = make_bo(GL_ELEMENT_ARRAY_BUFFER, &this->faces.edge_flat[0],
    this->faces.edge_flat.size()*sizeof(GLuint));
  bind_other(shader);
}

void MESH::bind_flat(GLuint shader){
  // bind vbo
  glBindVertexArray(this->vao_flat);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_flat);
  glBindAttribLocation(shader, POS_LOCATION, "vPosition");
  glEnableVertexAttribArray(POS_LOCATION);
  glVertexAttribPointer(POS_LOCATION, 3, GL_FLOAT,
                        GL_FALSE,
                        sizeof(VERTEX),
                        (GLvoid*)offsetof(VERTEX, pos));

  // Vertex Normals
  glBindAttribLocation(shader, NORMAL_LOCATION, "vNormal");
  glEnableVertexAttribArray(NORMAL_LOCATION);
  glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT,
                        GL_FALSE,
                        sizeof(VERTEX),
                        (GLvoid*)offsetof(VERTEX, normal));
}

void MESH::bind_other(GLuint shader){
  // bind vbo
  glBindVertexArray(this->vao_other);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_other);
  glBindAttribLocation(shader, POS_LOCATION, "vPosition");
  glEnableVertexAttribArray(POS_LOCATION);
  glVertexAttribPointer(POS_LOCATION, 3, GL_FLOAT,
                        GL_FALSE,
                        sizeof(VERTEX),
                        (GLvoid*)offsetof(VERTEX, pos));

  // Vertex Normals
  glBindAttribLocation(shader, NORMAL_LOCATION, "vNormal");
  glEnableVertexAttribArray(NORMAL_LOCATION);
  glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT,
                        GL_FALSE,
                        sizeof(VERTEX),
                        (GLvoid*)offsetof(VERTEX, normal));
}

void MESH::compute_face_normal(){
  glm::vec3 normal;
  // Assuming all vertices are given in counter-clock order
  for (unsigned int count=0; count<faces.draw_indices.size(); count+=3){
    normal = glm::cross(this->vertices[ this->faces.draw_indices[count + 2] ].pos
                      - this->vertices[ this->faces.draw_indices[count + 1] ].pos,
                        this->vertices[ this->faces.draw_indices[count    ] ].pos
                      - this->vertices[ this->faces.draw_indices[count + 1] ].pos );
    this->faces.normal.push_back(glm::normalize(normal));
    this->vertices_flat[count].normal = normal;
    this->vertices_flat[count+1].normal = normal;
    this->vertices_flat[count+2].normal = normal;
  }
}

void MESH::compute_vertex_normal(){
  vector<vector<int>> faces_per_vertex(this->num_v, vector<int>());
  glm::vec3 normal(0, 0, 0);
  int num_triangles = this->faces.draw_indices.size() / 3;
  int count = 0;
  for (int i=0; i<num_triangles; i++){  // which face we are looking at
    for (int j=0; j<3; j++){  // all vertices in this face
      //cout << this->faces.num_v[i] << endl;
      faces_per_vertex[this->faces.draw_indices[count]].push_back(i);
      count++;
    }
  }
  //for (int i=0; i<(int)this->num_v; i++)
  //for (auto k = faces_per_vertex[i].begin(); k != faces_per_vertex[i].end(); k++)
  //  cout << (*k) << ' ';
  //  cout << endl;
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
    /*
    cout << this->vertices[i].normal[0] << " "
         << this->vertices[i].normal[1] << " "
         << this->vertices[i].normal[2] << " " << endl;
         */
  }
}

void MESH::compute_light_product(LIGHT& THE_LIGHT) {
  this->ambient_product = glm::vec4(
                this->texture.ambient[0]*THE_LIGHT.ambient0[0],
                this->texture.ambient[1]*THE_LIGHT.ambient0[1],
                this->texture.ambient[2]*THE_LIGHT.ambient0[2], 1.0f);

  this->diffuse_product = glm::vec4(
                this->texture.diffuse[0]*THE_LIGHT.diffuse0[0],
                this->texture.diffuse[1]*THE_LIGHT.diffuse0[1],
                this->texture.diffuse[2]*THE_LIGHT.diffuse0[2], 1.0f);

  this->specular_product = glm::vec4(
                this->texture.specular[0]*THE_LIGHT.specular0[0],
                this->texture.specular[1]*THE_LIGHT.specular0[1],
                this->texture.specular[2]*THE_LIGHT.specular0[2], 1.0f);
  //cout << glm::to_string(ambient_product) << endl;
}

int read_mesh(string filename, MESH& mesh, int repeated_count, GLuint shader){
  glm::vec3 local_max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  glm::vec3 local_min = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
  vector<GLuint> holder_indices;
  string off;
  int vec_in_fac;
  int temp, temp_first;
  ifstream my_fin;
  my_fin.open(filename);
  if (!my_fin.is_open()){
    cerr << "READ_MESH: CAN NOT OPEN FILE: " << filename <<endl;
    return GLFW_FALSE;
  }
  getline(my_fin, off);
  if (off.compare("OFF") != 0){
    cerr << "READ_MESH: NOT AN OFF FILE: " << filename <<endl;
    return GLFW_FALSE;
  }
  /* reading attributs */
  my_fin >> mesh.num_v;
  my_fin >> mesh.num_f;
  my_fin >> mesh.num_e;
  /* reading vertices */
  mesh.vertices.resize(mesh.num_v);
  for (int i=0; i<(int)mesh.num_v; i++){
    for (int j=0; j<3; j++){
      my_fin >> mesh.vertices[i].pos[j];
    }
    local_max[0] = local_max[0]<mesh.vertices[i].pos[0]?mesh.vertices[i].pos[0]:local_max[0];
    local_max[1] = local_max[1]<mesh.vertices[i].pos[1]?mesh.vertices[i].pos[1]:local_max[1];
    local_max[2] = local_max[2]<mesh.vertices[i].pos[2]?mesh.vertices[i].pos[2]:local_max[2];
    local_min[0] = local_min[0]>mesh.vertices[i].pos[0]?mesh.vertices[i].pos[0]:local_min[0];
    local_min[1] = local_min[1]>mesh.vertices[i].pos[1]?mesh.vertices[i].pos[1]:local_min[1];
    local_min[2] = local_min[2]>mesh.vertices[i].pos[2]?mesh.vertices[i].pos[2]:local_min[2];
  }
  /* reading faces */
  int count = 0;
  int temp_count = 0;
  for (int i=0; i<(int)mesh.num_f; i++){
    my_fin >> vec_in_fac;
    mesh.faces.num_v.push_back(static_cast<GLuint>(vec_in_fac));
    if (vec_in_fac == 3){
      for (int j=0; j<vec_in_fac;j++){
        my_fin >> temp;
        mesh.faces.draw_indices.push_back(temp);
        mesh.vertices_flat.push_back(VERTEX());
        mesh.vertices_flat[count].pos = mesh.vertices[temp].pos;

        if (j == 0) {
          mesh.faces.edge_indices.push_back(temp);
          mesh.faces.edge_flat.push_back(count);
          temp_first = temp;
        } else {
          mesh.faces.edge_indices.push_back(temp);
          mesh.faces.edge_indices.push_back(temp);
          mesh.faces.edge_flat.push_back(count);
          mesh.faces.edge_flat.push_back(count);
        }
        count++;
      }
      mesh.faces.edge_indices.push_back(temp_first);
      mesh.faces.edge_flat.push_back(count-3);
    }else{ // when more than three vertices in a face
      for (int j=0; j<vec_in_fac;j++){
        my_fin >> temp;
        holder_indices.push_back(temp);

        if (j == 0) {
          mesh.faces.edge_indices.push_back(temp);
          temp_first = temp;
        } else {
          mesh.faces.edge_indices.push_back(temp);
          mesh.faces.edge_indices.push_back(temp);
        }
      }
      mesh.faces.edge_indices.push_back(temp_first);
      for (int j=0; j<(vec_in_fac-2);j++){
        mesh.faces.draw_indices.push_back(holder_indices[0]);
        mesh.faces.draw_indices.push_back(holder_indices[j+1]);
        mesh.faces.draw_indices.push_back(holder_indices[j+2]);
        mesh.vertices_flat.push_back(VERTEX());
        mesh.vertices_flat[count].pos = mesh.vertices[holder_indices[0]].pos;
        if (j == 0) {
          mesh.faces.edge_flat.push_back(count);
          temp_count = count;
        }
        count++;
        mesh.vertices_flat.push_back(VERTEX());
        mesh.vertices_flat[count].pos = mesh.vertices[holder_indices[j+1]].pos;
        mesh.faces.edge_flat.push_back(count);
        mesh.faces.edge_flat.push_back(count);
        count++;
        mesh.vertices_flat.push_back(VERTEX());
        mesh.vertices_flat[count].pos = mesh.vertices[holder_indices[j+2]].pos;
        mesh.faces.edge_flat.push_back(count);
        mesh.faces.edge_flat.push_back(count);
        count++;
      }
      mesh.faces.edge_flat.push_back(temp_count);
      holder_indices.clear(); // reset holder
    }
  }
  my_fin.close();
  for (int i = 0; i < repeated_count; i++) {
    float max_scale = -FLT_MAX;
    for (int j = 0; j < 3; j++) {
      max_scale = local_max[j]-local_min[j]>max_scale?local_max[j]-local_min[j]:max_scale;
    }
    glm::mat4 new_mat;
    new_mat = glm::scale(glm::vec3(
                         MESH_X/max_scale,
                         MESH_Y/max_scale,
                         MESH_Z/max_scale));
    new_mat = glm::translate(new_mat, glm::vec3(
                          max_scale*(mesh.id[i]%3)*BLOCK,
                          0,
                          max_scale*(mesh.id[i]/3)*BLOCK));
    
    mesh.scaled.push_back(new_mat);
    mesh.transforms.push_back(new_mat);
    mesh.spin.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
  }
  load_texture(mesh, material_props[MAT_TURQUOISE]);
  mesh.compute_face_normal();
  mesh.compute_vertex_normal();
  mesh.setup(shader);
  return GLFW_TRUE;
}

void read_all_meshes(map<string, int>& filenames, vector<MESH>& all_meshes, GLuint shader){
  int i = 0;
  int id_count = 0;
  for (auto itr_file = filenames.begin(); itr_file != filenames.end(); itr_file++){
    //cout << "reading " << filenames[i] << endl;
    for (int j = 0; j < itr_file->second; j++) {
      all_meshes[i].id.push_back(id_count);
      id_count++;
    }
    if(!read_mesh(itr_file->first, all_meshes[i], itr_file->second, shader)){
      id_count -= itr_file->second;
      all_meshes.erase(all_meshes.begin()+i);
    } else {
      i++;
    }
  }
}

void print_mesh_info(MESH& mesh){
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
}

void load_texture(MESH& mesh, const GLfloat* texture){
  mesh.texture.ambient[0] = texture[0];
  mesh.texture.ambient[1] = texture[1];
  mesh.texture.ambient[2] = texture[2];
  mesh.texture.diffuse[0] = texture[3];
  mesh.texture.diffuse[1] = texture[4];
  mesh.texture.diffuse[2] = texture[5];
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

void MESH::draw(GLuint shader, glm::mat4& PROJ_MAT, glm::mat4& MV_MAT, 
  LIGHT& THE_LIGHT, d_mode DRAW_MODE, s_mode SHADING_MODE) {
  glLinkProgram(shader);
  glUseProgram(shader);

  GLuint ambient = glGetUniformLocation(shader, "AmbientProduct");
  GLuint diffuse = glGetUniformLocation(shader, "DiffuseProduct");
  GLuint specular = glGetUniformLocation(shader, "SpecularProduct");
  GLuint shineness = glGetUniformLocation(shader, "Shineness");
  GLuint light = glGetUniformLocation(shader, "LightPosition");
  glUniform4fv(light, 1, glm::value_ptr(THE_LIGHT.light0));
  glUniform4fv(ambient, 1, glm::value_ptr(this->ambient_product));
  glUniform4fv(diffuse, 1, glm::value_ptr(this->diffuse_product));
  glUniform4fv(specular, 1, glm::value_ptr(this->specular_product));
  glUniform1f(shineness, this->texture.shineness);

  GLuint proj = glGetUniformLocation(shader, "Projection");
  glUniformMatrix4fv(proj, 1, GL_FALSE, glm::value_ptr(PROJ_MAT));
  GLuint mv = glGetUniformLocation(shader, "ModelView");
  glUniformMatrix4fv(mv, 1, GL_FALSE, glm::value_ptr(MV_MAT));
  GLuint trans = glGetUniformLocation(shader, "Transformation");
  for (unsigned int i = 0; i < id.size(); i++) {
    glUniformMatrix4fv(trans, 1, GL_FALSE, glm::value_ptr(transforms[i]));
    //cout << i <<": " << glm::to_string(transforms[i]) << endl;
    
    switch(DRAW_MODE) {
      case FACE:
        if (SHADING_MODE == FLAT) {
          glBindVertexArray(this->vao_flat);
          glBindBuffer(GL_ARRAY_BUFFER, this->vbo_flat);
          glDrawArrays(GL_TRIANGLES, 0, this->vertices_flat.size());
        } else {
          glBindVertexArray(this->vao_other);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_other);
          glDrawElements(GL_TRIANGLES, this->faces.draw_indices.size(), 
            GL_UNSIGNED_INT, (void*) 0);
        }
        break;

      case POINT:
      if (SHADING_MODE == FLAT) {
          glBindVertexArray(this->vao_flat);
          glBindBuffer(GL_ARRAY_BUFFER, this->vbo_flat);
          glDrawArrays(GL_POINTS, 0, this->vertices_flat.size());
        } else {
          glBindVertexArray(this->vao_other);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_other);
          glDrawElements(GL_POINTS, this->faces.draw_indices.size(), 
          GL_UNSIGNED_INT, (void*) 0);
        }
        break;

      case EDGE:
        if (SHADING_MODE == FLAT) {
          glBindVertexArray(this->vao_flat);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_flat);
          glDrawElements(GL_LINES, this->faces.edge_flat.size(), 
            GL_UNSIGNED_INT, (void*) 0);
        } else {
          glBindVertexArray(this->vao_other);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_edge);
          glDrawElements(GL_LINES, this->faces.edge_indices.size(), 
            GL_UNSIGNED_INT, (void*) 0);
        }
        break;

      default:
        break;
    }
  }
}

void MESH::update() {
  for (int i = 0; i < spin.size(); i++) {
    spin[i][0] += 1.0f;
    spin[i][1] -= 1.0f;
    spin[i][2] +=1.0f;
  }
}

void MESH::rotate() {
  for (unsigned int i = 0; i < transforms.size(); i++) {
    if (spin[i][0] > 720.0f)
      spin[i][0] -= 720.f;
    if (spin[i][1] < 720.0f)
      spin[i][1] += 720.f;
    if (spin[i][2] > 720.0f)
      spin[i][2] -= 720.f;

    transforms[i] = scaled[i];
    transforms[i] = glm::rotate(transforms[i], spin[i][0]*DEGREE_TO_RADIAN, 
      glm::vec3(1.0f, 0.0f, 0.0f));
    transforms[i] = glm::rotate(transforms[i], spin[i][1]*DEGREE_TO_RADIAN, 
      glm::vec3(0.0f, 1.0f, 0.0f));
    transforms[i] = glm::rotate(transforms[i], spin[i][2]*DEGREE_TO_RADIAN, 
      glm::vec3(0.0f, 0.0f, 1.0f));
  }
}
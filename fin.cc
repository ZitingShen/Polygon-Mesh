#include "fin.h"

void read_mesh(string filename, MESH& mesh){
  glm::vec3 holder;
  glm::vec4 holder_fac;
  string off;
  int vec_in_fac;
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
  for (int i=0; i<3; i++){
    my_fin >> mesh.attribute[i];
  }
  /* reading vertices */
  for (int i=0; i<mesh.attribute[0]; i++){
    for (int j=0; j<3; j++){
      my_fin >> holder[j];
    }
    mesh.vertices.push_back(holder);
  }
  /* reading faces */
  for (int i=0; i<mesh.attribute[1]; i++){
    my_fin >> vec_in_fac;
    mesh.vec_in_face.push_back(vec_in_fac);
    for (int j=0; j<vec_in_fac;j++){
      my_fin >> holder_fac[j];
    }
    holder_fac[3] = vec_in_fac==3?holder_fac[3]:0;
    mesh.faces.push_back(holder_fac);
  }
  my_fin.close();
}

void read_all_meshes(vector<string>& filenames, vector<MESH>& all_meshes){
  MESH a_mesh;
  for (int i=0; i<filenames.size(); i++){
    read_mesh(filenames[i], a_mesh);
    all_meshes.push_back(a_mesh);
  }  
}

void print_mesh_info(MESH& mesh){
  for (int i = 0; i<3; i++){
    cout << mesh.attribute[i] << " ";
  }
  cout << endl;

  for (int i = 0; i<mesh.attribute[0]; i++){
    for (int j=0; j<3; j++){
      cout << mesh.vertices[i][j] << " ";
    }
    cout << endl;
  }

  for (int i = 0; i<mesh.attribute[1]; i++){
    cout << mesh.vec_in_face[i] << " ";
    for (int j=0; j<mesh.vec_in_face[i]; j++){
      cout << mesh.faces[i][j] << " ";
    }
    cout << endl;
  }
}
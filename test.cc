#include "fin.h"

int main(){
	vector<MESH> all_meshes;
	vector<string> filenames;
	glm::vec3 max_xyz;
    glm::vec3 min_xyz;
	filenames.push_back("test.off");
	filenames.push_back("test2.off");
	all_meshes.reserve(filenames.size());
	all_meshes.resize(filenames.size());
	//filenames.push_back("test2.off");

	read_all_meshes(filenames, all_meshes, max_xyz, min_xyz);
	print_mesh_info(all_meshes[0]);
	cout << endl;
	print_mesh_info(all_meshes[1]);
	//cout << endl;
	//print_mesh_info(all_meshes[2]);
}
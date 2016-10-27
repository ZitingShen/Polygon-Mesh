#include "fin.h"

int main(){
	vector<MESH> all_meshes;
	vector<string> filenames;
	glm::vec3 max_xyz;
    glm::vec3 min_xyz;
	filenames.push_back("test.off");

	read_all_meshes(filenames, all_meshes, max_xyz, min_xyz);
	print_mesh_info(all_meshes[0]);
}
#include "fin.h"

int main(){
	vector<MESH> all_meshes;
	vector<string> filenames;
	filenames.push_back("test.off");

	read_all_meshes(filenames, all_meshes);
	print_mesh_info(all_meshes[0]);
}
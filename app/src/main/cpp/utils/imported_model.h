#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <android/asset_manager_jni.h>

class ImportedModel {

private:
	int vertices_num;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> tex_coords;
	std::vector<glm::vec3> normal_vecs;

public:
	ImportedModel();
	ImportedModel(AAssetManager *mgr, const char *file_path);
	int get_vetices_num();
	std::vector<glm::vec3> get_vertices();
	std::vector<glm::vec2> get_texture_coords();
	std::vector<glm::vec3> get_normals();
};


class ModelImporter {
private:
	std::vector<float> vert_vals;
	std::vector<float> triangle_verts;
	std::vector<float> texture_coords;
	std::vector<float> st_vals;
	std::vector<float> normals;
	std::vector<float> norm_vals;

public:
	ModelImporter();
	void parse_obj(AAssetManager *mgr, const char *file_path);
	int get_vertices_num();
	std::vector<float> get_vertices();
	std::vector<float> get_texture_coordinates();
	std::vector<float> get_normals();
};


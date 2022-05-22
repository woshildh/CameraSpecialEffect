#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "imported_model.h"
#include <android/asset_manager_jni.h>

using namespace std;
// ===============================================  ģ��������  ==============================
ImportedModel::ImportedModel() {}

ImportedModel::ImportedModel(AAssetManager *mgr, const char* file_path) {
	// ���� ModelImported ���� obj �ļ��Ķ�������
	ModelImporter importer;
	importer.parse_obj(mgr, file_path);
	// ��ȡ��������
	vertices_num = importer.get_vertices_num();
	std::vector<float> vs = importer.get_vertices();
	std::vector<float> tcs = importer.get_texture_coordinates();
	std::vector<float> ns = importer.get_normals();

	// ���Ƶ�����
	for (int i = 0; i < vertices_num; ++i) {
		vertices.push_back(glm::vec3(vs[i * 3], vs[i * 3 + 1], vs[i * 3 + 2]));
		tex_coords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normal_vecs.push_back(glm::vec3(ns[i * 3], ns[i * 3 + 1], ns[i * 3 + 2]));
	}
}

int ImportedModel::get_vetices_num() {
	return vertices_num;
}

std::vector<glm::vec3> ImportedModel::get_vertices() {
	return vertices;
}

std::vector<glm::vec2> ImportedModel::get_texture_coords() {
	return tex_coords;
}

std::vector<glm::vec3> ImportedModel::get_normals() {
	return normal_vecs;
}

// ===============================================  ģ�ͼ�����   ==============================
ModelImporter::ModelImporter() {}

void ModelImporter::parse_obj(AAssetManager *mgr, const char* file_path) {
	// ��ȡ�ļ�
    string content;
	{
        AAsset *asset = AAssetManager_open(mgr, file_path, AASSET_MODE_UNKNOWN);
        off_t length = AAsset_getLength(asset);
        content.reserve(length);
        char *buffer = (char*)malloc(length * sizeof(char) + 2);
        memset(buffer, 0, length + 2);
        AAsset_read(asset, buffer, length);
        AAsset_close(asset);
        content.assign(buffer);
        free(buffer);
    }

	float x, y, z;
	string line;
	stringstream fs(content, ios::in);
	while (!fs.eof()) {
		// ��ȡһ��
		getline(fs, line);
		// v -- ��ʾ��������
		if (line.compare(0, 2, "v ") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			vert_vals.push_back(x);
			vert_vals.push_back(y);
			vert_vals.push_back(z);
		}
		// vt ��ʾ�������������
		if (line.compare(0, 2, "vt") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			st_vals.push_back(x);
			st_vals.push_back(y);
		}
		// vn ��ʾ����ķ�����
		if (line.compare(0, 2, "vn") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			norm_vals.push_back(x);
			norm_vals.push_back(y);
			norm_vals.push_back(z);
		}
		// f ����������ƽ��Ķ�����Ϣ (������λ��/����λ��/������λ��)
		if (line.compare(0, 2, "f ") == 0) {
			string one_corner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; ++i) {
				getline(ss, one_corner, ' ');
				stringstream corner_ss(one_corner);
				getline(corner_ss, v, '/');
				getline(corner_ss, t, '/');
				getline(corner_ss, n, '/');

				// ���ַ�����idתΪ����
				int vert_id = (stoi(v) - 1) * 3;
				int vt_id = (stoi(t) - 1) * 2;
				int vn_id = (stoi(n) - 1) * 3;

				// ��������Ϣ������������Ϣ����������Ϣ����������
				triangle_verts.push_back(vert_vals[vert_id]);
				triangle_verts.push_back(vert_vals[vert_id + 1]);
				triangle_verts.push_back(vert_vals[vert_id + 2]);

				texture_coords.push_back(st_vals[vt_id]);
				texture_coords.push_back(st_vals[vt_id + 1]);

				normals.push_back(norm_vals[vn_id]);
				normals.push_back(norm_vals[vn_id + 1]);
				normals.push_back(norm_vals[vn_id + 2]);
			}
		}
	}
}

int ModelImporter::get_vertices_num() {
	return triangle_verts.size() / 3;
}

std::vector<float> ModelImporter::get_vertices() {
	return triangle_verts;
}

std::vector<float> ModelImporter::get_texture_coordinates() {
	return texture_coords;
}

std::vector<float> ModelImporter::get_normals() {
	return normals;
}


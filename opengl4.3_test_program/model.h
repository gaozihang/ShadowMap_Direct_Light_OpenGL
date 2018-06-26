#pragma once
#include "mesh.h"
#include "stb_image.h"

class Model
{
public:
	Model(char *path);
	void Draw(Shader shader);
	vector<Mesh> meshes;
	vector<Texture> texture_loaded;
private:
	string directory;
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	GLuint TextureFromFile(string directory, const char* path);
};
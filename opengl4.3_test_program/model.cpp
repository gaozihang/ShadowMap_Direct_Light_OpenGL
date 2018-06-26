#include "model.h"

Model::Model(char* path)
{
	loadModel(path);
}

void Model::Draw(Shader shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//cout << "loading Node: " << node->mName.C_Str() << endl;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		//cout << "--loading Mesh: " << mesh->mName.C_Str() << endl;
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	//cout << "----Mesh NumVertices: " << mesh->mNumVertices << endl;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Positon = position;
		//cout << "----vertex: (" << position.x << ", " << position.y << ", " << position.z << ")" << endl;
		glm::vec3 normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.Normal = normal;
		//cout << "----normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << endl;
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texcoord(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			vertex.TexCoords = texcoord;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		//cout << "----TexCoord: (" << vertex.TexCoords.x << ", " << vertex.TexCoords.y << ")" << endl;
		vertices.push_back(vertex);
	}
	//cout << "----Mesh NumFaces: " << mesh->mNumFaces << endl;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	//cout << "----material Number: " << mesh->mMaterialIndex + 1 << endl;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < texture_loaded.size(); j++)
		{
			if (strcmp(texture_loaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				//cout << "------texture : " << texture_loaded[j].path.C_Str() << endl;
				textures.push_back(texture_loaded[j]);
				skip = true;
				break;
			}
		}
		if (skip == false)
		{
			Texture texture;
			texture.id = TextureFromFile(directory, str.C_Str());
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			texture_loaded.push_back(texture);
			//cout << "------texture : " << texture.path.C_Str() << endl;
		}
	}
	return textures;
}

GLuint Model::TextureFromFile(string directory, const char* path)
{
	string filename = directory + '/' + string(path);
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int img_width, img_height, img_channels;
	unsigned char* image = stbi_load(filename.c_str(), &img_width, &img_height, &img_channels, STBI_rgb_alpha);
	if (image)
	{
		GLenum format;
		if (img_channels == 1)
			format = GL_RED;
		else if (img_channels == 3)
			format = GL_RGB;
		else if (img_channels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(image);
	}

	return textureID;
}
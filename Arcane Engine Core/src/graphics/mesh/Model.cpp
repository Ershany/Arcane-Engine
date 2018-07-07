#include "Model.h"

#include <iostream>
#include <stb_image_aug.h>
#include <iostream>

#include "../../platform/OpenGL/Utility.h"
#include "../../utils/Logger.h"

namespace arcane { namespace graphics {

	std::vector<Texture> Model::m_LoadedTextures;

	Model::Model(const char *path) {
		loadModel(path);
	}

	Model::Model(const std::vector<Mesh> &meshes) {
		m_Meshes = meshes;
	}

	void Model::Draw(Shader &shader) const {
		for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
			m_Meshes[i].Draw(shader);
		}
	}

	void Model::loadModel(const std::string &path) {
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			utils::Logger::getInstance().error("logged_files/model_loading.txt", "model initialization", import.GetErrorString());
			return;
		}

		m_Directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode *node, const aiScene *scene) {
		// Process all of the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			// Each node has an array of mesh indices, use these indices to get the meshes from the scene
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(processMesh(mesh, scene));
		}
		// Process all of the node's children
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces * 3); // Assume triangles. If not it will still load, it just won't be optimized

		// Process vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			glm::vec2 vec;
			// Texture Coordinates (check if there is texture coordinates)
			if (mesh->mTextureCoords[0]) {
				// A vertex can contain up to 8 different texture coordinates. We are just going to use one set of TexCoords per vertex so grab the first one
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
			}
			else {
				vec.x = 0.0f;
				vec.y = 0.0f;
			}

			// Construct the vertex object in the vector
			vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, vec.x, vec.y);
		}

		// Process Indices
		// Loop through every face (triangle thanks to aiProcess_Triangulate) and stores its indices in our meshes indices. This will ensure they are in the right order.
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// Process Materials (textures in this case)
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

			// grab all of the diffuse maps
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// grab all of the specular maps
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const char *typeName) {
		std::vector<Texture> textures;
		textures.reserve(mat->GetTextureCount(type));
		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			
			for (unsigned int j = 0; j < Model::m_LoadedTextures.size(); ++j) {
				if (std::strcmp(str.C_Str(), Model::m_LoadedTextures[j].path.C_Str()) == 0) {
					textures.push_back(Model::m_LoadedTextures[j]);
					skip = true;
					break;
				}
			}

			if (!skip) {
				Texture texture;
				
				texture.id = opengl::Utility::loadTextureFromFile((m_Directory + "/" + std::string(str.C_Str())).c_str()); // Assumption made: material stuff is located in the same directory as the model object
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				Model::m_LoadedTextures.push_back(texture); // Add to loaded textures, so no duplicate texture gets loaded
			}
		}
		return textures;
	}
} }
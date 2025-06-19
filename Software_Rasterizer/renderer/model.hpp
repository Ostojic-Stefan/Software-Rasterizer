#pragma once

#include <string_view>
#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "core.hpp"

namespace gfx
{
	struct model
	{
		model(std::string_view path)
		{
			load_model(path);
		}

		void load_model(std::string_view path)
		{
			Assimp::Importer imp;
			const aiScene* scene = imp.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

			ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode, "failed to load model");

			center_model(scene);

			process_node(scene->mRootNode, scene);
		}

		void process_node(const aiNode* node, const aiScene* scene)
		{
			for (rnd::u32 i = 0; i < node->mNumMeshes; ++i)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(process_mesh(mesh, scene));
			}

			// process children nodes
			for (rnd::u32 i = 0; i < node->mNumChildren; ++i)
			{
				process_node(node->mChildren[i], scene);
			}
		}

		mesh process_mesh(const aiMesh* m, const aiScene* scene)
		{
			std::vector<vertex> vertices;
			std::vector<rnd::u16> indices;

			for (rnd::u32 i = 0; i < m->mNumVertices; ++i)
			{
				vertex vert;
				vert.position = ass3_vec3(m->mVertices[i]);

				if (m->HasNormals())
				{
					vert.normal = ass3_vec3(m->mNormals[i]);
				}

				if (m->HasTextureCoords(0))
				{
					vert.tex_coords = ass3_vec2(m->mTextureCoords[0][i]);
					vert.tangent = ass3_vec3(m->mTangents[i]);
					vert.bitangent = ass3_vec3(m->mBitangents[i]);
				}
				else
				{
					vert.tex_coords = { 0.f, 0.f };
				}

				vertices.push_back(vert);
			}

			for (rnd::u32 i = 0; i < m->mNumFaces; ++i)
			{
				aiFace face = m->mFaces[i];

				ASSERT(face.mNumIndices == 3, "face should only have 3 indices");

				for (rnd::u32 j = 0; j < face.mNumIndices; ++j)
				{
					indices.push_back(face.mIndices[j]);
				}
			}

			return mesh(vertices, indices);
		}

	private:
		void center_model(const aiScene* scene)
		{
			aiVector3D centroid(0, 0, 0);
			size_t verts = 0;
			for (rnd::u32 m = 0; m < scene->mNumMeshes; ++m)
			{
				const aiMesh* mesh = scene->mMeshes[m];
				for (rnd::u32 v = 0; v < mesh->mNumVertices; ++v)
				{
					centroid += mesh->mVertices[v];
					++verts;
				}
			}
			centroid /= (rnd::f32) (verts);
			for (rnd::u32 m = 0; m < scene->mNumMeshes; ++m)
			{
				aiMesh* mesh = scene->mMeshes[m];
				for (rnd::u32 v = 0; v < mesh->mNumVertices; ++v)
				{
					mesh->mVertices[v] -= centroid;
				}
			}
		}

		math::vec3 ass3_vec3(aiVector3D v)
		{
			return { v.x, v.y, v.z };
		}

		math::vec2 ass3_vec2(aiVector3D v)
		{
			return { v.x, v.y };
		}

	public:
		std::vector<mesh> meshes;
	};
}


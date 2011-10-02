#pragma once

#include "../../Candy_Dll.h"
#include "ResourceInfo.h"
#include "Resource.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "../Mesh.h"
#include "../Animation.h"
#include <Candy/Tools/Ptr.h>
#include <map>

namespace Candy
{

	class ResourcePool
	{
	public:
		static Ptr(ResourcePool) Singleton;

		CANDY_API static void Initialize(const std::string& asset_dir);

	private:
		ResourcePool(const std::string& asset_dir);
		ResourcePool(const ResourcePool&);
		ResourcePool& operator=(const ResourcePool&);

	public:
		~ResourcePool();

		Ptr(ShaderProgram) Get(Ptr(ShaderInfo) id);
		Ptr(TextureSingle) Get(Ptr(TextureId) id);
		Ptr(Ressource) Get(Ptr(MeshId) id);
		Ptr(Animation) Get(Ptr(MeshAnimId) id);

	public:
		Ptr(ShaderInfo) Load(const std::string& vertex_fn, const std::string& geometry_fn, const std::string& fragment_fn, const std::vector<std::string>& attributes, const std::vector<std::string>& bindings);
		Ptr(ShaderInfo) Load(const std::string& vertex_fn, const std::string& fragment_fn, const std::vector<std::string>& attributes, const std::vector<std::string>& bindings);

	private:
		Ptr(Ressource) GetImpl(Ptr(ResourceId) id);
		Ptr(Ressource) Create(Ptr(ResourceId) id);
		Ptr(ShaderProgram) CreateShader(Ptr(ShaderInfo) id);
		Ptr(TextureSingle) CreateTexture(Ptr(TextureId) id);
		Ptr(Ressource) CreateMesh(Ptr(MeshId) id);
		Ptr(Animation) CreateMeshAnimation(Ptr(MeshAnimId) id);

	private:
		std::string assets_dir_;
		std::map<Ptr(ResourceId),Ptr(Ressource)> resources_;
	};

}

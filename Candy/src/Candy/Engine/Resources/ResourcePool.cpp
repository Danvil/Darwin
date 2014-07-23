#include "ResourcePool.h"
#include "MeshFactory.h"
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

namespace Candy
{
	Ptr(ResourcePool) ResourcePool::Singleton;

	void ResourcePool::Initialize(const std::string& asset_dir)
	{
		Singleton.reset(new ResourcePool(asset_dir));
	}

	ResourcePool::ResourcePool(const std::string& asset_dir)
	{
		assets_dir_ = asset_dir;
	}

	ResourcePool::~ResourcePool()
	{
	}

	Ptr(ShaderProgram) ResourcePool::Get(Ptr(ShaderInfo) id)
	{
		return std::dynamic_pointer_cast<ShaderProgram>(GetImpl(id));
	}

	Ptr(TextureSingle) ResourcePool::Get(Ptr(TextureId) id)
	{
		return std::dynamic_pointer_cast<TextureSingle>(GetImpl(id));
	}

	Ptr(Ressource) ResourcePool::Get(Ptr(MeshId) id)
	{
		return std::dynamic_pointer_cast<Ressource>(GetImpl(id));
	}

	Ptr(Animation) ResourcePool::Get(Ptr(MeshAnimId) id)
	{
		return std::dynamic_pointer_cast<Animation>(GetImpl(id));
	}

	string readFileContents( const std::string& filename )
	{
		// open file
		std::ifstream ifs(filename.c_str());
		if( !ifs.is_open() ) {
			throw std::runtime_error("File '" + filename + "' could not be opened for reading");
		}
		// read file
		string contents = "";
		string line;
		while( !ifs.eof() ) {
			getline(ifs, line);
			contents += line + "\n";
		}
		// close file and return
		ifs.close();
		return contents;
	}

	Ptr(ShaderInfo) ResourcePool::Load(const std::string& vertex_fn, const std::string& geometry_fn, const std::string& fragment_fn, const std::vector<std::string>& attributes, const std::vector<std::string>& bindings)
	{
		Ptr(ShaderInfo) info(new ShaderInfo());
		info->geometry_src = readFileContents(assets_dir_ + geometry_fn);
		info->vertex_src = readFileContents(assets_dir_ + vertex_fn);
		info->fragment_src = readFileContents(assets_dir_ + fragment_fn);
		info->attributes = attributes;
		info->bindings = bindings;
		return info;
	}

	Ptr(ShaderInfo) ResourcePool::Load(const std::string& vertex_fn, const std::string& fragment_fn, const std::vector<std::string>& attributes, const std::vector<std::string>& bindings)
	{
		Ptr(ShaderInfo) info(new ShaderInfo());
		info->geometry_src = ""; // no geometry shader
		info->vertex_src = readFileContents(assets_dir_ + vertex_fn);
		info->fragment_src = readFileContents(assets_dir_ + fragment_fn);
		info->attributes = attributes;
		info->bindings = bindings;
		return info;
	}

	Ptr(Ressource) ResourcePool::GetImpl(Ptr(ResourceId) id)
	{
		// check if existing
		for(std::map<Ptr(ResourceId),Ptr(Ressource)>::iterator it=resources_.begin(); it!=resources_.end(); ++it) {
			if(id->equals(it->first)) {
				return it->second;
			}
		}
		// create new
		Ptr(Ressource) r = Create(id);
		resources_[id] = r;
		return r;
	}

	Ptr(Ressource) ResourcePool::Create(Ptr(ResourceId) id)
	{
		switch(id->type()) {
		case Shader: return CreateShader(std::dynamic_pointer_cast<ShaderInfo>(id));
		case Texture: return CreateTexture(std::dynamic_pointer_cast<TextureId>(id));
		case Mesh: return CreateMesh(std::dynamic_pointer_cast<MeshId>(id));
		case MeshAnim: return CreateMeshAnimation(std::dynamic_pointer_cast<MeshAnimId>(id));
		default: throw 0;
		}
	}

	Ptr(ShaderProgram) ResourcePool::CreateShader(Ptr(ShaderInfo) id)
	{
		return Ptr(ShaderProgram)(new ShaderProgram(id));
	}

	Ptr(TextureSingle) ResourcePool::CreateTexture(Ptr(TextureId) id)
	{
		Texture2* raw = new Texture2();
		if(id->filename_.length() == 0) {
			raw->LoadTest(256, 256);
		}
		else if(id->filename_ == "test") {
			raw->LoadTest(256, 256);
		}
		else if(id->filename_ == "white") {
			raw->LoadWhite(16, 16);
		}
		else {
			raw->Load(assets_dir_ + id->filename_);
		}
		return Ptr(TextureSingle)(raw);
	}

	Ptr(Ressource) ResourcePool::CreateMesh(Ptr(MeshId) id)
	{
		switch(id->mesh_type_) {
		case MeshP:		return MeshFactory::LoadObjP(assets_dir_ + id->filename_);
		case MeshPN:	return MeshFactory::LoadObjPN(assets_dir_ + id->filename_);
		case MeshPTN:	return MeshFactory::LoadObjPTN(assets_dir_ + id->filename_);
		default: throw std::runtime_error("Invalid mesh type!");
		}
	}

	Ptr(Animation) ResourcePool::CreateMeshAnimation(Ptr(MeshAnimId) id)
	{
		size_t raute_pos = id->pattern_.find('#');
		std::string p1 = id->pattern_.substr(0, raute_pos);
		std::string p2 = id->pattern_.substr(raute_pos + 1, id->pattern_.length() - raute_pos - 1);
		std::vector<Ptr(IDrawable)> ticks;
		for(size_t i=1; ; i++) {
			std::stringstream ss;
			ss << p1 << std::setw(6) << std::setfill('0') << i << p2;
			std::string fn = ss.str();
//			std::cout << "Loading " << fn << std::endl;
			{ std::ifstream test((assets_dir_ + fn).c_str());
				if(!test) {
					break;
				}
			}
			Ptr(Ressource) mesh = Get(Ptr(MeshId)(new MeshId(id->mesh_type_, fn)));
			ticks.push_back(std::dynamic_pointer_cast<IDrawable>(mesh));
		}
		return Ptr(Animation)(new Animation(ticks));
	}

}

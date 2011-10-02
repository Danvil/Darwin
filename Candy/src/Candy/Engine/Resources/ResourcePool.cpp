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
		return Ptr(ShaderProgram)(GetImpl(id), boost::detail::dynamic_cast_tag());
	}

	Ptr(TextureSingle) ResourcePool::Get(Ptr(TextureId) id)
	{
		return Ptr(TextureSingle)(GetImpl(id), boost::detail::dynamic_cast_tag());
	}

	Ptr(Ressource) ResourcePool::Get(Ptr(MeshId) id)
	{
		return Ptr(Ressource)(GetImpl(id), boost::detail::dynamic_cast_tag());
	}

	Ptr(Animation) ResourcePool::Get(Ptr(MeshAnimId) id)
	{
		return Ptr(Animation)(GetImpl(id), boost::detail::dynamic_cast_tag());
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
		case Shader: return CreateShader(Ptr(ShaderInfo)(id, boost::detail::dynamic_cast_tag()));
		case Texture: return CreateTexture(Ptr(TextureId)(id, boost::detail::dynamic_cast_tag()));
		case Mesh: return CreateMesh(Ptr(MeshId)(id, boost::detail::dynamic_cast_tag()));
		case MeshAnim: return CreateMeshAnimation(Ptr(MeshAnimId)(id, boost::detail::dynamic_cast_tag()));
		default: throw 0;
		}
	}

	Ptr(ShaderProgram) ResourcePool::CreateShader(Ptr(ShaderInfo) id)
	{
		ShaderProgram* raw = new ShaderProgram(id);
		Ptr(ShaderProgram) x(raw);
		return x;
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
		Ptr(TextureSingle) x(raw);
		return x;
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
			ticks.push_back(Ptr(IDrawable)(mesh, boost::detail::dynamic_cast_tag()));
		}
		return Ptr(Animation)(new Animation(ticks));
	}

}

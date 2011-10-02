#pragma once

#include <Candy/Tools/Ptr.h>
#include <string>
#include <vector>

namespace Candy
{

	enum ResourceType
	{
		Shader,
		Texture,
		Mesh,
		MeshAnim
	};

	enum MeshType
	{
		MeshP,
		MeshPN,
		MeshPTN
	};

	//typedef unsigned int Id;

	class ResourceId
	{
	public:
		ResourceId() : is_unique_(false) {}
		ResourceType type() const {
			return type_;
		}
		bool IsUnique() const {
			return is_unique_;
		}
		/*Id id() const {
			return id_;
		}*/
		virtual bool equals(const Ptr(ResourceId)& r);
	protected:
		ResourceType type_;
		bool is_unique_;
		//Id id_;
	public:
		//Id CreateId() const;
	};

	class ShaderInfo
		: public ResourceId
	{
	public:
		ShaderInfo() {
			type_ = Shader;
		}

		virtual bool equals(const Ptr(ResourceId)& r);

	public:
		std::string vertex_src;
		std::string geometry_src;
		std::string fragment_src;
		std::vector<std::string> attributes;
		std::vector<std::string> bindings;

	};

	class TextureId
		: public ResourceId
	{
	public:
		TextureId() {
			type_ = Texture;
		}
		TextureId(const std::string& fn)
		: filename_(fn) {
			type_ = Texture;
		}
		virtual bool equals(const Ptr(ResourceId)& r);
	public:
		std::string filename_;
	};

	class MeshId
		: public ResourceId
	{
	public:
		MeshId(MeshType mt) {
			mesh_type_ = mt;
			type_ = Mesh;
		}
		MeshId(MeshType mt, const std::string& fn)
		: filename_(fn) {
			mesh_type_ = mt;
			type_ = Mesh;
		}
		virtual bool equals(const Ptr(ResourceId)& r);
	public:
		MeshType mesh_type_;
		std::string filename_;
	};

	class MeshAnimId
		: public ResourceId
	{
	public:
		MeshAnimId(MeshType mt) {
			mesh_type_ = mt;
			type_ = MeshAnim;
		}
		MeshAnimId(MeshType mt, const std::string& pattern)
		: pattern_(pattern) {
			mesh_type_ = mt;
			type_ = MeshAnim;
		}
		virtual bool equals(const Ptr(ResourceId)& r);
	public:
		MeshType mesh_type_;
		std::string pattern_;
	};

}

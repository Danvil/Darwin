#include "ResourcePool.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>
using namespace std;

namespace Candy
{

	/*Id ResourceId::CreateId() const
	{
		static Id next = 0;
		return next++;
	}*/

	bool ResourceId::equals(const Ptr(ResourceId)& r)
	{
		if(this->IsUnique() || r->IsUnique()) {
			return false;
		}
		if(r->type() != this->type()) {
			return false;
		}
		return true;
	}

	bool ShaderInfo::equals(const Ptr(ResourceId)& r)
	{
		if(!ResourceId::equals(r)) {
			return false;
		}
		Ptr(ShaderInfo) theirs(r, boost::detail::dynamic_cast_tag());
		return this->vertex_src == theirs->vertex_src
			&& this->geometry_src == theirs->geometry_src
			&& this->fragment_src == theirs->fragment_src;
		// FIXME check attributes and bindings (but should be no problem ...)
	}

	bool TextureId::equals(const Ptr(ResourceId)& r)
	{
		if(!ResourceId::equals(r)) {
			return false;
		}
		Ptr(TextureId) theirs(r, boost::detail::dynamic_cast_tag());
		return this->filename_ == theirs->filename_;
	}

	bool MeshId::equals(const Ptr(ResourceId)& r)
	{
		if(!ResourceId::equals(r)) {
			return false;
		}
		Ptr(MeshId) theirs(r, boost::detail::dynamic_cast_tag());
		return this->mesh_type_ == theirs->mesh_type_
			&& this->filename_ == theirs->filename_;
	}

	bool MeshAnimId::equals(const Ptr(ResourceId)& r)
	{
		if(!ResourceId::equals(r)) {
			return false;
		}
		Ptr(MeshAnimId) theirs(r, boost::detail::dynamic_cast_tag());
		return this->mesh_type_ == theirs->mesh_type_
			&& this->pattern_ == theirs->pattern_;
	}

}

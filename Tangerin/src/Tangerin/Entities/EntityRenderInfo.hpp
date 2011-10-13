/*
 * EntityRenderInfo.hpp
 *
 *  Created on: Oct 13, 2011
 *      Author: david
 */

#ifndef ENTITYRENDERINFO_HPP_
#define ENTITYRENDERINFO_HPP_

#include <Candy/Tools/Ptr.h>
#include <string>
#include <map>

namespace Tangerin {

struct EntityRenderInfo
{
	std::string tag_;
	std::string fn_mesh_;
	std::string fn_texture_ao_;
	std::string fn_texture_emit_;

	EntityRenderInfo(const std::string& tag, const std::string& fn_mesh)
	: tag_(tag), fn_mesh_(fn_mesh) {}

	EntityRenderInfo(const std::string& tag, const std::string& fn_mesh, const std::string& fn_texture_ao)
	: tag_(tag), fn_mesh_(fn_mesh), fn_texture_ao_(fn_texture_ao) {}

	EntityRenderInfo(const std::string& tag, const std::string& fn_mesh, const std::string& fn_texture_ao, const std::string& fn_texture_emit)
	: tag_(tag), fn_mesh_(fn_mesh), fn_texture_ao_(fn_texture_ao), fn_texture_emit_(fn_texture_emit) {}

};

struct EntityRenderInfoFactory
{
public:
	static Ptr(EntityRenderInfoFactory) GetSingleton() {
		return s_factory_;
	}

	Ptr(EntityRenderInfo) registerInfo(const EntityRenderInfo& info);

	Ptr(EntityRenderInfo) getInfo(const std::string& tag);

private:
	static Ptr(EntityRenderInfoFactory) s_factory_;

	std::map<std::string, Ptr(EntityRenderInfo)> data_;
};

}

#endif

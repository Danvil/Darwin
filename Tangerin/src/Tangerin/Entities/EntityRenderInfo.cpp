/*
 * EntityRenderInfo.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: david
 */

#include "EntityRenderInfo.hpp"
#include <stdexcept>

namespace Tangerin {

Ptr(EntityRenderInfoFactory) EntityRenderInfoFactory::s_factory_(new EntityRenderInfoFactory());

Ptr(EntityRenderInfo) EntityRenderInfoFactory::registerInfo(const EntityRenderInfo& info)
{
	if(data_.find(info.tag_) != data_.end()) {
		throw std::runtime_error("Duplicated render info with tag " + info.tag_ + "'!");
	}
	Ptr(EntityRenderInfo) rip(new EntityRenderInfo(info));
	data_[rip->tag_] = rip;
	return rip;
}

Ptr(EntityRenderInfo) EntityRenderInfoFactory::getInfo(const std::string& tag)
{
	auto it = data_.find(tag);
	if(it == data_.end()) {
		throw std::runtime_error("No render info with tag '" + tag + "'!");
	}
	return it->second;
}

}

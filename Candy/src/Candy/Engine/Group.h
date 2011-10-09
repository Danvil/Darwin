#pragma once

#include "Node.h"
#include "Shader.h"
#include <boost/function.hpp>

namespace Candy
{
	/** One visual, one shader, n instances realized with uniform buffers */
	template<typename T>
	class Group
		: public INodeBase
	{
	public:
		typedef boost::function<void(size_t, const Ptr(Candy::ShaderX)&)> ApplyFunc;
		typedef boost::function<bool(size_t)> TestFunc;

		ApplyFunc OnApply;
		TestFunc OnTest;

		const T& at(size_t i) const {
			return data_[i];
		}

		T& at(size_t i) {
			return data_[i];
		}

		void Render()
		{
			shader_->ApplyStart();
			visual_->RenderStart();
			IDrawable::sCamera->Push();
			for(size_t i=0; i<data_.size(); i++) {
				if(!OnTest || OnTest(i)) {
					OnApply(i, shader_);
					shader_->ApplyTick();
					visual_->RenderTick();
				}
			}
			IDrawable::sCamera->Pop();
			shader_->ApplyStop();
			visual_->RenderStop();
		}

	protected:
		Ptr(Candy::IDrawable) visual_;
		Ptr(Candy::ShaderX) shader_;
		std::vector<T> data_;
	};
}

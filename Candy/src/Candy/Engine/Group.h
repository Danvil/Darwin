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
		typedef boost::function<void(size_t, const Ptr(Candy::ShaderX)&, const Mat4f& base)> ApplyFunc;
		typedef boost::function<bool(size_t, const Mat4f& base)> TestFunc;

		ApplyFunc OnApply;
		TestFunc OnTest;

		const T& at(size_t i) const {
			return data_[i];
		}

		T& at(size_t i) {
			return data_[i];
		}

		void Render(const Mat4f& m)
		{
			shader_->ApplyStart();
			visual_->RenderStart();
			for(size_t i=0; i<data_.size(); i++) {
				if(!OnTest || OnTest(i, m)) {
					OnApply(i, shader_, m);
					shader_->ApplyTick();
					visual_->RenderTick();
				}
			}
			shader_->ApplyStop();
			visual_->RenderStop();
		}

	protected:
		Ptr(Candy::IDrawable) visual_;
		Ptr(Candy::ShaderX) shader_;
		std::vector<T> data_;
	};
}

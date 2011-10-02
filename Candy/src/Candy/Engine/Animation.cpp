#include "Animation.h"

namespace Candy
{
	Animation::Animation(const std::vector<Ptr(IDrawable)>& ticks)
		: ticks_(ticks)
	{
		animation_time_ = 0.0f;
//		p_speed_ = 1.0f;
//		p_offset_ = 0.0f;
	}

	Animation::~Animation()
	{
	}

	void Animation::Generate()
	{
	}

	void Animation::Delete()
	{
	}

	Ptr(IDrawable) Animation::Get(float p)
	{
		int i = int(p * float(ticks_.size()));
		i = i % ticks_.size();
		return ticks_[i];
	}

	void Animation::Render()
	{
		if(IsEmpty()) {
			return;
		}
		//animation_time_ = p_speed_ * (p_offset_ + GetCurrentTime());
		Get(animation_time_)->Render();
	}
}

#pragma once

#include "Resources/Resource.h"
#include "IDrawable.h"

namespace Candy
{
	class Animation
	: public Ressource,
	  public IDrawable
	{
	public:
		Animation(const std::vector<Ptr(IDrawable)>& ticks);

		~Animation();

		void Generate();

		void Delete();

		bool IsEmpty() const {
			return ticks_.size() == 0;
		}

		Ptr(IDrawable) Get(float p);

		void Render();

		float animation_time_;

		//float p_speed_;

		//float p_offset_;

	private:
		std::vector<Ptr(IDrawable)> ticks_;
	};

}

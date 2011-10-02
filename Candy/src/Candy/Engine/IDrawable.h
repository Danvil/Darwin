#pragma once

#include "../Candy_Dll.h"
#include "Camera.hpp"
#include <Candy/Tools/Ptr.h>

namespace Candy
{
	class CANDY_API IDrawable
	{
	public:
		virtual ~IDrawable() {}

		virtual void RenderStart() {}

		virtual void RenderTick() {
			Render();
		}

		virtual void RenderStop() {}

		virtual void Render() {
			RenderStart();
			RenderTick();
			RenderStop();
		}

		static float GetCurrentTime();

		static Ptr(Camera) sCamera;
	};

}

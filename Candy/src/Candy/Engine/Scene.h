#pragma once

#include "../Candy_Dll.h"
#include "Node.h"
#include "Camera.hpp"
#include <Candy/Tools/Ptr.h>

namespace Candy
{
	class CANDY_API Scene
		: public Node
	{
	public:
		Scene();
		~Scene();

		Ptr(Camera) GetCamera() const {
			return camera_;
		}

		void Render();

	private:
		Ptr(Camera) camera_;
	};

}

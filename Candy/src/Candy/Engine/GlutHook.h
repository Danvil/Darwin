#pragma once

#include "../Candy_Dll.h"
#include "IEngine.h"
#include <boost/shared_ptr.hpp>
#include <string>

namespace Candy
{
	namespace GlutHook
	{
		CANDY_API void Initialize(const std::string& caption, boost::shared_ptr<IEngine> engine);
		
		CANDY_API void Run(int argc, char** argv);

	}
}

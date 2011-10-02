#include "Tangerin.h"
#include <Candy/Engine/GlutHook.h>
#include <Candy/Tools/Benchmark.h>
#include <iostream>

int main(int argc, char ** argv)
{
	if(argc != 2) {
		std::cout << "Usage: CubesDemo asset_path" << std::endl;
		return 1;
	}
	boost::shared_ptr<Tangerin::TangerinMain> core(new Tangerin::TangerinMain(argv[1]));
	Candy::GlutHook::Initialize("Tangerin", core);
	Candy::GlutHook::Run(argc, argv);
	DANVIL_BENCHMARK_PRINTALL_COUT
//	std::cin.get();
	return 0;
}

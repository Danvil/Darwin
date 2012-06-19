#include "Indoor.h"
#include <Candy/Engine/GlutHook.h>
#include <Candy/Tools/Benchmark.h>
#include <iostream>

int main(int argc, char ** argv)
{
	if(argc != 2) {
		std::cout << "Usage: Indoor asset_path" << std::endl;
		return 1;
	}
	boost::shared_ptr<IndoorMain> core(new IndoorMain(argv[1]));
	Candy::GlutHook::Initialize("Darwin / Indoor", core);
	Candy::GlutHook::Run(argc, argv);
	DANVIL_BENCHMARK_PRINTALL_COUT
//	std::cin.get();
	return 0;
}

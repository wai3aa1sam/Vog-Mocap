#pragma once

#ifdef VOG_PLATFORM_WINDOWS

extern vog::Application* vog::createApplication();

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef VOG_DEBUG
	#define DBG_NEW new ( _CLIENT_BLOCK , __FILE__ , __LINE__ )
	// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
	// allocations to be of _CLIENT_BLOCK type
#else
	#define DBG_NEW new
#endif

int main(int argc_, char** argv_)
{
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

	_CrtMemState checkPoint1;
	_CrtMemCheckpoint(&checkPoint1);

	{
		vog::Log::init();
		auto app = vog::createApplication();
		app->run();
		delete app;
	}

	_CrtMemState checkPoint2;
	_CrtMemCheckpoint(&checkPoint2);

	_CrtMemState diff;
	if (_CrtMemDifference(&diff, &checkPoint1, &checkPoint2)) {
		_CrtMemDumpStatistics(&diff);
	}
	// no leak actually, the leak is reported when import glad to our project
	// 5792 bytes in 112 Normal Blocks.
	// 5824 bytes in 114 Normal Blocks.
	// 5904 bytes in 119 Normal Blocks.

	return 0;
}

#endif // VOG_PLATFORM_WINDOWS

#ifdef EMSCRIPTEN
#include "WebApp.h"
#else
#include "WindowApp.h"
#endif

int main()
{
#ifdef EMSCRIPTEN
	BaseApp* app = new WebApp();
#else
	BaseApp* app = new WindowApp();
#endif
	const bool result = app->Run();
	delete app;

	return result ? 1 : 0;
}
#include <Vog.h>
#include "Vog/Core/StartupEntry.h"

#include <ImGui/imgui.h>

#include "MocapLayer.h"

class VogMocapApp : public vog::Application
{
public:
	VogMocapApp()
		:
		Application("Vog Mocap")
	{
		pushLayer(new vog::MocapLayer("Mocap Layer"));
	}
private:

};

vog::Application* vog::createApplication()
{
	return new VogMocapApp();
}
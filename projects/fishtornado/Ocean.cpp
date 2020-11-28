#include "Ocean.h"
#include "FishTornado.h"

Ocean::Ocean()
{
}

Ocean::~Ocean()
{
}

void Ocean::Setup(uint32_t numFramesInFlight)
{
}

void Ocean::Shutdown()
{
}

void Ocean::Update(uint32_t frameIndex)
{
    const float t = FishTornadoApp::GetThisApp()->GetTime();
    const float dt = FishTornadoApp::GetThisApp()->GetDt();
}

void Ocean::Render(grfx::CommandBuffer* pCmd)
{
}

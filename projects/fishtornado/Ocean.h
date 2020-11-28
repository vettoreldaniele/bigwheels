#ifndef OCEAN_H
#define OCEAN_H

#include "ppx/grfx/grfx_model.h"
using namespace ppx;

class FishTornadoApp;

class Ocean
{
public:
    Ocean();
    ~Ocean();

    void Setup(uint32_t numFramesInFlight);
    void Shutdown();
    void Update(uint32_t frameIndex);
    void Render(grfx::CommandBuffer* pCmd);

private:
};

#endif // OCEAN_H

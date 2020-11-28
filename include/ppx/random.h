#ifndef ppx_random_h
#define ppx_random_h

#include "ppx/000_math_config.h"
#include "pcg32.h"

namespace ppx {

class Random
{
public:
    Random()
    {
        Seed(0xDEAD, 0xBEEF);
    }

    Random(uint64_t initialState, uint64_t initialSequence)
    {
        Seed(initialSequence, initialSequence);
    }

    ~Random() {}

    void Seed(uint64_t initialState, uint64_t initialSequence)
    {
        mRng.seed(initialSequence, initialSequence);
    }

    uint32_t UInt32()
    {
        uint32_t value = mRng.nextUInt();
        return value;
    }

    float Float()
    {
        float value = mRng.nextFloat();
        return value;
    }

    float Float(float a, float b)
    {
        float value = Lerp(a, b, mRng.nextFloat());
        return value;
    }

    float2 Float2() {}

    float2 Float2(const float2& a, const float2& b) {}

    float3 Float3() {}

    float3 Float3(const float3& a, const float3& b) {}

    float4 Float4() {}

    float4 Float4(const float4& a, const float4& b) {}

private:
    pcg32 mRng;
};

} // namespace ppx

#endif // ppx_random_h

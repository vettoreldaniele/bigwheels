// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "config.hlsli"

// Undefine to disable manual filtering.
#define MANUAL_FILTERING

SamplerState ClampedSampler : register(s0);

struct PSInput
{
    float2 vUv : VUV;
};

// FIXME(dnovillo) - This should sample from these textures.
#if 0
Texture2D uVelocity : register(t0);

Texture2D uSource : register(t1);
#endif

float4 bilerp(Texture2D sam, float2 uv, float2 tsize)
{
    float2 st = uv / tsize - 0.5;

    float2 iuv = floor(st);
    float2 fuv = frac(st);

    float4 a = sam.SampleLevel(Sampler, (iuv + float2(0.5, 0.5)) * tsize, 0);
    float4 b = sam.SampleLevel(Sampler, (iuv + float2(1.5, 0.5)) * tsize, 0);
    float4 c = sam.SampleLevel(Sampler, (iuv + float2(0.5, 1.5)) * tsize, 0);
    float4 d = sam.SampleLevel(Sampler, (iuv + float2(1.5, 1.5)) * tsize, 0);

    return lerp(lerp(a, b, fuv.x), lerp(c, d, fuv.x), fuv.y);
}

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord inputCoord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

#ifdef MANUAL_FILTERING
    // FIXME(dnovillo) - Use uVelocity instead of InputTexture.
    float2 coord = inputCoord.uv - Params.dt * bilerp(InputTexture, inputCoord.uv, Params.texelSize).xy * Params.texelSize;
    // FIXME(dnovillo) - Use uSource instead of InputTexture.
    float4 result = bilerp(InputTexture, coord, Params.dyeTexelSize);
#else
    // FIXME(dnovillo) - Use uVelocity instead of InputTexture.
    float2 coord = inputCoord.uv - Params.dt * InputTexture.SampleLevel(Sampler, inputCoord.uv, 0).xy * Params.texelSize;
    // FIXME(dnovillo) - Use uSource instead of InputTexture.
    float4 result = InputTexture.SampleLevel(Sampler, coord, 0);
#endif
    float decay = 1.0 + Params.dissipation * Params.dt;

    OutputTexture[inputCoord.xy] = result / decay;
}

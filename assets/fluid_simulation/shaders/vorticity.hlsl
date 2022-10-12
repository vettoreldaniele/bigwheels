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

// FIXME(dnovillo) - Sample from these textures instead of InputTexture.
#if 0
Texture2D uVelocity : register(t0);
Texture2D uCurl : register(t1);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    // FIXME(dnovillo) - Sample from uCurl
    float L = InputTexture.SampleLevel(Sampler, coord.uvL, 0).x;
    float R = InputTexture.SampleLevel(Sampler, coord.uvR, 0).x;
    float T = InputTexture.SampleLevel(Sampler, coord.uvT, 0).x;
    float B = InputTexture.SampleLevel(Sampler, coord.uvB, 0).x;
    float C = InputTexture.SampleLevel(Sampler, coord.uv, 0).x;

    float2 force = 0.5 * float2(abs(T) - abs(B), abs(R) - abs(L));
    force /= length(force) + 0.0001;
    force *= Params.curl * C;
    force.y *= -1.0;

    // FIXME(dnovillo) - Sample from uVelocity.
    float2 velocity = InputTexture.SampleLevel(Sampler, coord.uv, 0).xy;
    velocity += force * Params.dt;
    velocity = min(max(velocity, -1000.0), 1000.0);

    OutputTexture[coord.xy] = float4(velocity, 0.0, 1.0);
}

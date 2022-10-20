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

// FIXME(dnovillo) - This should sample the uTarget texture.
#if 0
Texture2D uTarget : register(t0);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    float2 p = coord.uv - Params.coordinate.xy;
    p.x *= Params.aspectRatio;
    float3 splat = exp(-dot(p, p) / Params.radius) * Params.color.rgb;
    // FIXME(dnovillo) - Sample from uTarget.
    float3 base = InputTexture.SampleLevel(Sampler, coord.uv, 0).xyz;

    OutputTexture[coord.xy] = float4(base + splat, 1.0);
}

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

#define SCALE 25.0

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float2 uv    = floor(coord.uv * SCALE * float2(Params.aspectRatio, 1.0));
    float  v     = fmod(uv.x + uv.y, 2.0);
    v            = v * 0.1 + 0.8;

    OutputTexture[coord.xy] = float4(float3(v, v, v), 1.0);
}

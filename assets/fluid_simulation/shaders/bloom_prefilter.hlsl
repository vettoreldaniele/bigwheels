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

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float3 c     = InputTexture.SampleLevel(Sampler, coord.uv, 0).rgb;
    float  br    = max(c.r, max(c.g, c.b));
    float  rq    = clamp(br - Params.curve.x, 0.0, Params.curve.y);
    rq           = Params.curve.z * rq * rq;
    c *= max(rq, br - Params.threshold) / max(br, 0.0001);
    OutputTexture[coord.xy] = float4(c, 0.0);
}

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
    float4 sum   = 0.0;
    sum += InputTexture.SampleLevel(Sampler, coord.uvL, 0);
    sum += InputTexture.SampleLevel(Sampler, coord.uvR, 0);
    sum += InputTexture.SampleLevel(Sampler, coord.uvT, 0);
    sum += InputTexture.SampleLevel(Sampler, coord.uvB, 0);
    sum *= 0.25;
    OutputTexture[coord.xy] = sum;
}
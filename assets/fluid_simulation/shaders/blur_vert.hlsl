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

struct VSInput
{
    float2 aPosition : POSITION;
};

struct PSInput
{
    float2 vUv : VUV;
    float2 vL : VL;
    float2 vR : VR;
    float4 pos : POSITION;
};

cbuffer texelSize : register(b0)
{
    float2 texelSize;
};

PSInput vsmain(VSInput input)
{
    PSInput output;
    output.vUv   = input.aPosition * 0.5 + 0.5;
    float offset = 1.33333333;
    output.vL    = output.vUv - texelSize * offset;
    output.vR    = output.vUv + texelSize * offset;
    output.pos   = float4(input.aPosition, 0.0, 1.0);
    return output;
}

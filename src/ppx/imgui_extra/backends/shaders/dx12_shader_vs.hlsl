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

cbuffer vertexBuffer : register(b0) 
{
  float4x4 ProjectionMatrix; 
};

struct VS_INPUT
{
  float2 pos : POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};

struct PS_INPUT
{
  float4 pos : SV_POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
  PS_INPUT output;
  output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
  output.col = input.col;
  output.uv  = input.uv;
  return output;
}
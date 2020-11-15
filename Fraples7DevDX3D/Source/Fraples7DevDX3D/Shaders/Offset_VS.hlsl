#include "TransformData.hlsli"

cbuffer Offset
{
    float offset;
};

float4 main( float3 pos : POSITION, float3 n : Normal) : SV_POSITION
{
    return mul(float4(pos + n * offset, 1.0f), modelViewProj);
}
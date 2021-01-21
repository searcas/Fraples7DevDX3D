#include "TransformData.hlsli"
#include "VShadow.hlsli"
struct VSOut
{
	float3 viewPos : Position;
    float3 normal: Normal;
	float2 tc : Texcoord;
	float4 shadowHomePos : ShadowPosition;
	float4 pos : SV_Position;
};
VSOut main(float3 pos : POSITION,float3 normal : Normal, float2 tc : Texcoord) 
{
	VSOut vso;
	vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
	vso.normal = mul(normal, (float3x3) modelView);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	vso.tc = tc;
	vso.shadowHomePos = ToShadowScreenSpace(pos, model);
	return vso;
}
#include "TransformData.hlsli"
#include "VShadow.hlsli"
struct VSOut
{
	float3 viewPos : Position;
	float3 normal : Normal;
    noperspective float3 shadowCamScreen : ShadowPosition;
	float4 pos : SV_Position;
};
VSOut main(float3 pos : POSITION, float3 normal : Normal)
{
	VSOut vso;
	vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
	vso.normal = mul(normal, (float3x3) modelView);
    vso.shadowCamScreen = ToShadowScreenSpace(pos, model);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}
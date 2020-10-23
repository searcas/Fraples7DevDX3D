cbuffer Cbuf
{
	matrix modelView;
	matrix modelViewProj;
	
};

struct VSOut
{
	float3 worldPos : Position;
	float3 normal   : Normal;
	float3 color    : Color;
	float4 position : SV_Position;
};
VSOut main( float3 position : Position, float3 normal : Normal, float3 color : Color)
{
	VSOut vso;
	vso.worldPos = (float3) mul(float4(position, 1.0f), modelView);
	vso.normal = mul(normal, (float3x3) modelView);
	vso.position = mul(float4(position, 1.0f), modelViewProj);
	vso.color = color;
	return vso;
}
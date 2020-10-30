cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};
struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float2 texc : Texcoord;
    float4 pos : SV_Position;
};
VSOut main(float3 pos : POSITION, float3 normal : Normal, float3 tangent : Tangent, float3 bitan : Bitangent, float2 texc : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(normal, (float3x3) modelView);
    vso.tangent = mul(tangent, (float3x3)modelView);
    vso.bitangent = mul(bitan, (float3x3)modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.texc = texc;
    return vso;
}
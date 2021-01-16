
struct Out
{
    float4 color : Color;
    float4 pos : SV_Position;
};

Out main(float3 pos : POSITION, float3 color : Color)
{
    Out vsout;
    vsout.pos = float4(pos, 1.0f);
    vsout.color = float4(color, 1.0f);
    
    return vsout;
}
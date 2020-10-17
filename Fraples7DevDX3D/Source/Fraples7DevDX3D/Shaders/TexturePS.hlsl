Texture2D tex;

SamplerState splr;

float4 main(float2 texCord : TexCoord) : SV_TARGET
{
	return tex.Sample(splr,texCord);
}
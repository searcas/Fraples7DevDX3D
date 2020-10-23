cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState splr;

float4 main(float3 worldPos : Position, float3 n:Normal,float2 tc : Texcoord) : SV_TARGET
{
    //fragment to light vector data
    const float3 vTol = lightPos - worldPos;
    const float distTol = length(vTol);
    const float3 dirTol = vTol / distTol;
    
    //attenutation
    const float att = 1.0f / (attConst + attLin * distTol + attQuad * (distTol * distTol));
    
    //diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirTol, n));
    //reflected light vector
    const float3 w = n * dot(vTol, n);
    const float3 r = w * 2.0f - vTol;
    //calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power func
    
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    
    //final color 
    return float4(saturate(diffuse + ambient + specular), 1.0f) * tex.Sample(splr, tc);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
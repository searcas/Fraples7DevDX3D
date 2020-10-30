//https://docs.microsoft.com/en-us/cpp/preprocessor/intrinsic?view=vs-2019


cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;

//http://wiki.ogre3d.org/-Point+Light+Attenuation More info
    
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};


cbuffer ObjectCBuf
{
    
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

cbuffer TransformCBuf
{
    matrix modelView;
    matrix modelViewProj;
    
};

Texture2D tex;
Texture2D normalMap : register(t2);
SamplerState samplr;

float4 main(float3 worldPos : Position, float3 normal : Normal, float3 tangent : Tangent, float3 bitan : Bitangent, float2 texCoord : Texcoord) : SV_TARGET
{
    
    //sample normal from map if normal mappping enabled
    if (normalMapEnabled)
    {
        const float3x3 tanToView = float3x3(normalize(tangent), normalize(bitan), normalize(normal));
        
        const float3 normalMapSample = normalMap.Sample(samplr, texCoord).xyz;
        normal.x = normalMapSample.x * 2.0f - 1.0f;
        normal.y = -normalMapSample.y * 2.0f + 1.0f;
        normal.z = normalMapSample.z * 2.0f - 1.0f;
        //bring normal from tanspace into view space
        normal = mul(normal, tanToView);
    }

    //fragment to light vector data
    const float3 vTol = lightPos - worldPos;
    const float distTol = length(vTol);
    const float3 dirTol = vTol / distTol;
    //attenuation
    const float att = 1.0f / (attConst + attLin * distTol + attQuad * (distTol * distTol));
    
    //diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirTol, normal));

    //reflect light vector
    const float3 w = normal * dot(vTol, normal);
    const float3 r = w * 2.0f - vTol;
    //caclulate specular intensity based on angle between viewing vector 
    //and reflection vector, narrow with power func
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    
    //final color
    return float4(saturate((diffuse + ambient) * tex.Sample(samplr, texCoord).rgb + specular), 1.0f);
}
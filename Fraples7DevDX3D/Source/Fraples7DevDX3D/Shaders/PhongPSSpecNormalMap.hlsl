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
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWight;
};
Texture2D tex;
Texture2D spec;
Texture2D normalmap;

SamplerState samplr;

float4 main(float3 viewPos : Position, float3 normal : Normal, float3 tangent : Tangent, float3 bitan : Bitangent, float2 texcoord : Texcoord) : SV_TARGET
{

    if (normalMapEnabled)
    {
        const float3x3 tanToView = float3x3(normalize(tangent), normalize(bitan), normalize(normal));
        //sample normal from map if normal mappping enabled
        const float3 normalMapSample = normalmap.Sample(samplr, texcoord).xyz;
        float3 tanNormal;
        tanNormal = normalMapSample * 2.0f - 1.0f;
        //bring normal from tanspace into view space
        normal = mul(tanNormal, tanToView);
    }
        
    //fragment to light vector data
    const float3 vTol = lightPos - viewPos;
    const float distTol = length(vTol);
    const float3 dirTol = vTol / distTol;
    //attenuation
    const float att = 1.0f / (attConst + attLin * distTol + attQuad * (distTol * distTol));
    
    //diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirTol, normal));

    //reflect light vector
    const float3 w = normal * dot(vTol, normal);
    const float3 r = w * 2.0f - vTol;
    
    //caclulate specular intensity based on angle between viewing vector and reflection vector, narrow with power func
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
       const float4 specularSample = spec.Sample(samplr, texcoord);
        specularReflectionColor = specularSample.rgb * specularMapWight;
        if (hasGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
    //final color
    return float4(saturate((diffuse + ambient) * tex.Sample(samplr, texcoord).rgb + specular * specularReflectionColor), 1.0f);
}
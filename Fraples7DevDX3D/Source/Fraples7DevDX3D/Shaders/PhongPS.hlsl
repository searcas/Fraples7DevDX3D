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
    float padding[2];
};
Texture2D tex;
SamplerState samplr;



float4 main(float3 viewPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_TARGET
{
    viewPos = normalize(viewPos);
    //fragment to light vector data
    const float3 vTol = lightPos - viewPos;
    const float distTol = length(vTol);
    const float3 dirTol = vTol / distTol;
    //attenuation
    const float att = 1.0f / (attConst + attLin * distTol + attQuad * (distTol * distTol));
    
    //diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirTol, n));

    //reflect light vector
    const float3 w = n * dot(vTol, n);
    const float3 r = w * 2.0f - vTol;
    //caclulate specular intensity based on angle between viewing vector and reflection vector, narrow with power func
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * 
    pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
    
    //final color
    return float4(saturate((diffuse + ambient) * tex.Sample(samplr, tc).rgb + specular), 1.0f);
}
#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specularPowerConst;
    bool hasGloss;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;

SamplerState splr;
float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal,float2 texcoord : Texcoord) : SV_TARGET
{
    //normalize the mesh normal
    viewNormal = normalize(viewNormal);
    //fragemnt to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    //specular parameters
    float specularPower = specularPowerConst;
    const float4 specularSample = spec.Sample(splr, texcoord);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    if(hasGloss)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    
    //attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    //diffuse light
    const float3 diffuse = Diffuse(diffuseColor,diffuseIntensity,att,lv.dirToL,viewNormal);
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lv.vToL, viewFragPos, att, specularPower);
    
    //final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, texcoord).rgb + specularReflected), 1.0f);
}
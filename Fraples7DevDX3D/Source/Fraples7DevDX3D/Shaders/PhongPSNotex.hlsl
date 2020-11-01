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
	float4 materialColor;
	float4 specularColor;
	float specularPower;

};
float4 main(float3 viewPos : Position, float3 normal : Normal) : SV_TARGET
{
	
	//renormalize interpolated normal
    viewPos = normalize(viewPos);
	const float3 vTol = lightPos - viewPos;
    const float distTol = length(vTol);
    const float3 dirTol = vTol / distTol;
	
	//attenuation
    const float attenuation = 1.0f / (attConst + attLin * distTol + attQuad * (distTol * distTol));
	//diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(dirTol, normal));
	
    const float3 w = normal * dot(vTol, normal);
    const float3 r = w * 2.0f - vTol;
	
	//calculate specular intensity based on angle between viewing  vector and reflection vector, narrow with power function
	
    const float4 specular = attenuation * (float4(diffuseColor, 1.0f) * specularColor) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	
	//final color
	

    return saturate(float4(diffuse + ambient, 1.0f) * materialColor + specular);

}
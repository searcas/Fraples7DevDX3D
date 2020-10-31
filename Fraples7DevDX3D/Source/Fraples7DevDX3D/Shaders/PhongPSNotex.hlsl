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
	float3 materialColor;
	float specularIntensity;
	float specularPower;
	float padding[2];
};
float4 main(float3 viewPos : Position, float3 normal : Normal) : SV_TARGET
{
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
	
    const float3 specular = attenuation * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	
	//final color
	

    return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);

}
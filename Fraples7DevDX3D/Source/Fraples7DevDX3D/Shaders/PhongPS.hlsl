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
    float3 materialColor;
};
float4 main(float3 worldPos : Position, float3 n : Normal) : SV_TARGET
{
	//fragment to light vector data
	const float3 vTol = lightPos - worldPos;
	const float distTol = length(vTol);
	const float3 dirTol = vTol / distTol;
	//diffuse attenuation
	const float att = 1.0f / (attConst + attLin * distTol + attQuad * (distTol * distTol));
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirTol, n));

	//final color
	return float4(saturate(diffuse + ambient) * materialColor,1.0f);
}
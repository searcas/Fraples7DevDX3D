//https://docs.microsoft.com/en-us/cpp/preprocessor/intrinsic?view=vs-2019

cbuffer LightCBuf
{
	float3 lightPos;
};

static const float3 materialColor = { 0.7f,0.7f,0.9f };
static const float3 ambient = { 0.05f,0.05f,0.05f };
static const float3 diffuseColor = { 1.0f,1.0f,1.0f };

//http://wiki.ogre3d.org/-Point+Light+Attenuation More info
static const float diffuseIntensity = 1.0f;
static const float attConst= 1.0f;
static const float attLin= .45f;
static const float attQuad = 0.0075f;

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
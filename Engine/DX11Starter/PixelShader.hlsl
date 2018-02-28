
struct DirectionalLight {
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};
cbuffer externalData : register(b0) {
	DirectionalLight light;
	DirectionalLight lightTwo;
}

//Textures
Texture2D DiffuseTexture : register(t0);
SamplerState Samp : register(s0);

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};


// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 dir = normalize(-light.Direction);
	float dirLightAmount = saturate(dot(input.normal, dir));

	float3 dirTwo = normalize(-lightTwo.Direction);
	float dirLightAmountTwo = saturate(dot(input.normal, dirTwo));

	float4 surfaceColor = DiffuseTexture.Sample(Samp, input.uv);


	return surfaceColor * ((light.DiffuseColor * dirLightAmount) + light.AmbientColor +
		(lightTwo.DiffuseColor * dirLightAmountTwo) + lightTwo.AmbientColor);

}
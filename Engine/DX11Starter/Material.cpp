#include "Material.h"




Material::Material(SimpleVertexShader * VertexShader,
	SimplePixelShader * PixelShader, 
	ID3D11ShaderResourceView* Texture, 
	ID3D11SamplerState* Sampler)
{
	vertexShader = VertexShader;
	pixelShader = PixelShader;
	sampler = Sampler;
	texture = Texture;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::getTexture()
{
	return texture;
}

ID3D11SamplerState * Material::getSampler()
{
	return sampler;
}


Material::~Material()
{
}

#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "SimpleShader.h"

class Material
{
public:
	Material(SimpleVertexShader* VertexShader, SimplePixelShader* PixelShader, ID3D11ShaderResourceView* Texture, ID3D11SamplerState* Sampler);
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* getTexture();
	ID3D11SamplerState* getSampler();
	~Material();
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
	ID3D11SamplerState* sampler;

};


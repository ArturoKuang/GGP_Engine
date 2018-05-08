#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Material.h"
#include "ParticleSystem.h"

using namespace DirectX;

class Entities
{
public:
	Entities(Mesh* Mesh, Material* Material);
	void SetTranslation(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	XMFLOAT4X4 GetWorldMatrix();
	void Draw(ID3D11DeviceContext* context, DXGI_FORMAT format, UINT strideSize);
	void Move(float totalTime);
	void PerpareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 projection);
	void UpdateCloth(float timer, ID3D11DeviceContext* device, VertexPosColor* vertices);
	void SetParticleSystem(ParticleSystem* p_System);
private:
	ParticleSystem* particleSystem;
	Material* material;
	Mesh* mesh;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
};


#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Material.h"

using namespace DirectX;

class Entities
{
public:
	Entities(Mesh* Mesh, Material* Material);
	void SetTranslation(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	XMFLOAT4X4 GetWorldMatrix();
	void Draw(ID3D11DeviceContext* context);
	void Move(float totalTime);
	void PerpareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 projection);
private:
	Material* material;
	Mesh* mesh;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
};


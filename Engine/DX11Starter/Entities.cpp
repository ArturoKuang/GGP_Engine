#include "Entities.h"


Entities::Entities(Mesh * Mesh, Material* Material)
{
	material = Material;
	mesh = Mesh;
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	position = { 1, 1, 0 };
	scale = { 1, 1, 1 };
	rotation = { 0, 0, 0 };
}

void Entities::SetTranslation(float x, float y, float z)
{
	position = { x, y, z };
}

void Entities::SetRotation(float x, float y, float z)
{
	rotation = { x, y, z };
}

void Entities::SetScale(float x, float y, float z)
{
	scale = { x, y, z };
}

XMFLOAT4X4 Entities::GetWorldMatrix()
{
	XMMATRIX tr = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX ro = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX sc = XMMatrixScaling(scale.x, scale.y, scale.z);
	// Store the matrix so it's sent to the GPU during draw
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(sc * ro * tr));
	return worldMatrix;
}

void Entities::Draw(ID3D11DeviceContext * context, DXGI_FORMAT format, UINT strideSize)
{
	//UINT stride = sizeof(VertexPosColor);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &strideSize, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), format, 0);
	//context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);
	//draw
	context->DrawIndexed(
		mesh->GetIndexCount(),
		0,
		0
	);

}

void Entities::Move(float totalTime)
{
	float sinTime = sin(totalTime * 2);
	position.x = sinTime;
	//rotation = { 0, 0, XM_PI * totalTime };
	scale = { .25f , .25f, .25f };
}

void Entities::PerpareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 projection)
{
	//set vertex shader
	material->GetVertexShader()->SetMatrix4x4("view", view);
	material->GetVertexShader()->SetMatrix4x4("projection", projection);
	material->GetVertexShader()->SetMatrix4x4("world", GetWorldMatrix());
	material->GetVertexShader()->SetShader();
	material->GetVertexShader()->CopyAllBufferData();
	//set pixel shader
	material->GetPixelShader()->SetSamplerState("Samp", material->getSampler());
	material->GetPixelShader()->SetShaderResourceView("DiffuseTexture", material->getTexture());
	material->GetPixelShader()->CopyAllBufferData();
	material->GetPixelShader()->SetShader();
}

void Entities::UpdateCloth(float timer, ID3D11DeviceContext* device, VertexPosColor* vertices)
{
	XMFLOAT3* pEdge = particleSystem->GetEdge();
	static float animationCounter = .0f;
	for (int32_t ii = 0; ii < 32; ii++)
	{
		pEdge[ii].z = 1.f * sinf(animationCounter);
	}
	animationCounter += .125f * timer;

	particleSystem->Update(timer);

	//copy particles pos to vertex pos
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//disable gpu access to the vertex buffer data
	device->Map(mesh->GetVertexBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	uint32_t ii = 0;
	for (uint32_t zz = 0; zz < 32; zz++) 
	{
		for (uint32_t xx = 0; xx < 32; xx++) 
		{
			vertices[ii].pos = particleSystem->GetParticlesPos(ii);
			ii++;
		}
	}
	memcpy(mappedResource.pData, mesh->GetClothVertices(), mesh->GetClothVerticesSize());

	//reenable GPU access to the vertex buffer data
	device->Unmap(mesh->GetVertexBuffer(), 0);
}

void Entities::SetParticleSystem(ParticleSystem * p_System)
{
	particleSystem = p_System;
}


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

void Entities::Draw(ID3D11DeviceContext * context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
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


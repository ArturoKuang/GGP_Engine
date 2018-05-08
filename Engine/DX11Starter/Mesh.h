#pragma once
#include <d3d11.h>
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace DirectX;

class Mesh
{
public:
	Mesh(char* fileName, ID3D11Device* device);
	Mesh(VertexPosColor vertices[],
		int vertexCount,
		unsigned short indices[], 
		int indexCount, 
		ID3D11Device* device);

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	VertexPosColor* GetClothVertices();
	int GetClothVerticesSize();

	int GetIndexCount();
	void CreateBuffers(
		Vertex vertices[],
		int vertexCount, 
		unsigned int indices[], 
		int indexCount, 
		ID3D11Device* device);
	void CreateClothBuffers(
		VertexPosColor vertices[],
		int vertexCount,
		unsigned short indices[],
		int indexCount,
		ID3D11Device* device);
	~Mesh();
private:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	VertexPosColor* clothVertices;
	int clothVerticesSize;
	int indexBufferCount;
};




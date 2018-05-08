#include "Mesh.h"

Mesh::Mesh(
	VertexPosColor vertices[], 
	int vertexCount, 
	unsigned short indices[], 
	int indexCount, 
	ID3D11Device* device)
{
	clothVertices = vertices;
	clothVerticesSize = vertexCount;
	CreateClothBuffers(clothVertices, clothVerticesSize, indices, indexCount, device);
}

Mesh::Mesh(char * fileName, ID3D11Device* device)
{
	//file input stream
	ifstream obj(fileName);

	//check for sucessful open 
	if (!obj.is_open())
		return;

	//variables for reading file
	std::vector<XMFLOAT3> positions;  
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<Vertex> verts; //verts we're assembling 
	std::vector<UINT> indices; //indices of these verts
	unsigned int vertCounter = 0; //count of vertices/indices
	char chars[100];

	while (obj.good()) {
		//get the line (100 charcthers should be more then enough)
		obj.getline(chars, 100);

		//check the type of line
		if (chars[0] == 'v' && chars[1] == 'n') {
			//reads the 3 numbers directly into XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(chars, "vn %f %f %f", &norm.x, &norm.y, &norm.z);

			//add to the list of normals 
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't') {
			//reads the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(chars, "vt %f %f", &uv.x, &uv.y);
			//add to the list of uvs 
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v') {
			//reads the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(chars, "v %f %f %f", &pos.x, &pos.y, &pos.z);
			//add to list
			positions.push_back(pos);
		}
		else if (chars[0] == 'f') {
			//reads the face indices into an array 
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars, 
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);


			//create the verts by looking up corresponding 
			//data from vectors 
			//-OBJ file indces are 1-based, so they need to be 
			//adjusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			//the model is most likely in a right-handed space
			//especially if it came from maya. we want to convert
			//to left-handed space form directX. this means we need
			//to: 
			//-invert the z pos
			//-invert the normal's z
			//-flip the winding order 

			//flips uvs
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;
			//flip z
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;
			//flip normal z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;

			//add the verts back
			verts.push_back(v1);
			verts.push_back(v2);
			verts.push_back(v3);

			//add three more indices
			indices.push_back(vertCounter); vertCounter++;
			indices.push_back(vertCounter); vertCounter++;
			indices.push_back(vertCounter); vertCounter++;

			//was there a 4th face
			if (facesRead == 12) {

				//make the last vertex
				Vertex v4;
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];

				//flip the uv, z pos and normal
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;

				//add the whole triagnle 
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);

				//add three more indices
				indices.push_back(vertCounter); vertCounter++;
				indices.push_back(vertCounter); vertCounter++;
				indices.push_back(vertCounter); vertCounter++;
			}
			
		}
	}
	obj.close();
	CreateBuffers(&verts[0], vertCounter, &indices[0], vertCounter, device);
}

ID3D11Buffer * Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer * Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

VertexPosColor * Mesh::GetClothVertices()
{
	return clothVertices;
}

int Mesh::GetClothVerticesSize()
{
	return clothVerticesSize;
}

int Mesh::GetIndexCount()
{
	return indexBufferCount;
}

void Mesh::CreateBuffers(
	Vertex vertices[], 
	int vertexCount, 
	unsigned int indices[], 
	int indexCount, 
	ID3D11Device * device)
{
	indexBufferCount = indexCount;
	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;       // 3 = number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	// Create the INDEX BUFFER description ------------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indexCount;         // 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
}

void Mesh::CreateClothBuffers(VertexPosColor vertices[], int vertexCount, unsigned short indices[], int indexCount, ID3D11Device * device)
{
	indexBufferCount = indexCount;
	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = vertexCount;       // 3 = number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
	initialVertexData.SysMemPitch = 0;
	initialVertexData.SysMemSlicePitch = 0;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	// Create the INDEX BUFFER description ------------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = indexCount;         // 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;
	initialIndexData.SysMemPitch = 0;
	initialIndexData.SysMemSlicePitch = 0;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
}


Mesh::~Mesh()
{
	if (vertexBuffer) { vertexBuffer->Release(); }
	if (indexBuffer) { indexBuffer->Release(); }
}

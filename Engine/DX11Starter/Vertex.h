#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The position of the vertex
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;

};

//cloth struct 
struct VertexPosColor {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};
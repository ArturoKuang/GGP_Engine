#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entities.h"
#include "Camera.h"
#include "LIghts.h"
#include "ParticleSystem.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	VertexPosColor* clothVertices;
	unsigned short* clothIndices;
	//lights
	DirectionalLight light;
	DirectionalLight lightTwo;
	//mesh objects 
	Mesh* sphere;
	Mesh* cloth;
	//entity list 
	std::vector<Entities*> entityList;
	//camera
	Camera* camera;
	//materials
	Material* material;
	Material* wickMaterial;
	//textures
	ID3D11ShaderResourceView* clothTexture;
	ID3D11ShaderResourceView* wickTexture;
	ID3D11SamplerState* samplerState;
	//particle system 
	ParticleSystem* m_particleSystem;

	
	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};


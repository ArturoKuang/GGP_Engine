#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexBuffer = 0;
	indexBuffer = 0;
	vertexShader = 0;
	pixelShader = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (vertexBuffer) { vertexBuffer->Release(); }
	if (indexBuffer) { indexBuffer->Release(); }
	if (obj) { delete obj;  }
	if (obj_1) { delete obj_1; }
	if (obj_2) { delete obj_2; }
	//release entities 
	for (int i = 0; i < entityList.size(); i++) {
		delete entityList[i];
	}
	//release texture
	if (samplerState) { samplerState->Release(); samplerState = 0; }
	if (clothTexture) { clothTexture->Release(); clothTexture = 0; }

	//release material
	delete material;
	//release canera
	delete camera;
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();
	//intialize light
	light.AmbientColor = XMFLOAT4(.50f, 0.0f, 0.0f, 1.0f);
	light.DiffuseColor = XMFLOAT4(.50f, 0.0f, 0.0f, 1.0f);
	light.Direction = XMFLOAT3(1.0f, -1.0f, .0f);

	lightTwo.AmbientColor = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	lightTwo.DiffuseColor = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	lightTwo.Direction = XMFLOAT3(1.0f, 1.0f, .0f);

	//intialize camera
	camera = new Camera();
	camera->SetProjectionMatrix((float)width / height);
	//load texture
	CreateWICTextureFromFile(device, context, L"Textures/Wirnkles.jpg", 0, &clothTexture);
	CreateWICTextureFromFile(device, context, L"Textures/Wicker.jpg", 0, &wickTexture);
	//sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc = {}; 
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &samplerState);

	//intialize materials
	material = new Material(vertexShader, pixelShader, clothTexture, samplerState);
	wickMaterial = new Material(vertexShader, pixelShader, wickTexture, samplerState);
	//intialize entities
	entityList.push_back(new Entities(obj, material));
	entityList.push_back(new Entities(obj_1, wickMaterial));
	entityList[0]->SetTranslation(1, 1, 0);
	entityList[1]->SetTranslation(1, 2, 0);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	//square
	Vertex squareVerticies[] =
	{
		{ XMFLOAT3(+3.0f, +0.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f)},
		{ XMFLOAT3(+3.0f, -0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+2.5f, -0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },

		{ XMFLOAT3(+2.5f, -0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+2.5f, +0.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+3.0f, +0.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },

	};
	unsigned int sqaureIndices[] = { 0, 1, 2, 3, 4, 5 };

	//some shape
	Vertex shapeVerticies[] =
	{	
		
		{ XMFLOAT3(+3.0f, +1.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+3.0f, +0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+2.5f, +0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
							 
		{ XMFLOAT3(+2.5f, +0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+2.5f, +1.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+3.0f, +1.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		
		
		{ XMFLOAT3(+2.75f, +1.75f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+3.0f, +1.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
		{ XMFLOAT3(+2.5f, +1.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
	};

	unsigned int shapeIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8};

	obj = new Mesh("Models/sphere.obj", device);
	obj_1 = new Mesh("Models/sphere.obj", device);
	obj_2 = new Mesh(shapeVerticies, 9, shapeIndices, 9, device);

}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	camera->SetProjectionMatrix((float) width / height);

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
	//move entities 
	for (int i = 0; i < entityList.size(); i++) {
		entityList[i]->Move(totalTime + i);
		worldMatrix = entityList[i]->GetWorldMatrix();
	}
	camera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);


	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	//vertexShader->SetMatrix4x4("world", worldMatrix);
	for (int i = 0; i < entityList.size(); i++) {
		entityList[i]->PerpareMaterial(camera->GetViewMatrix(), camera->GetProjectionMatrix());
		entityList[i]->Draw(context);
	}
	pixelShader->SetData("light", &light, sizeof(DirectionalLight));
	pixelShader->SetData("lightTwo", &lightTwo, sizeof(DirectionalLight));
	pixelShader->CopyAllBufferData(); // Remember to copy to the GPU!!!!
	pixelShader->SetShader();
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Check left mouse button
	if (buttonState & 0x0001)
	{
		float xDiff = (x - prevMousePos.x) * 0.005f;
		float yDiff = (y - prevMousePos.y) * 0.005f;
		camera->Rotate(xDiff, yDiff);
	}

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion
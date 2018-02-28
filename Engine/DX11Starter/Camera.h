#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <Windows.h> //keyboard input

using namespace DirectX;

class Camera
{
public:
	Camera();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();
	void Update(float deltaTime);
	void Rotate(float x, float y);
	void SetProjectionMatrix(float aspectRatio);
	~Camera();
private:
	XMVECTOR position;
	XMVECTOR direction;
	XMVECTOR up;
	float rotateX;
	float rotateY;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
};


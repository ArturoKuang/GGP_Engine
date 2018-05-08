#include "Camera.h"



Camera::Camera()
{
	position = XMVectorSet(0, 0, -5, 0);
	direction = XMVectorSet(0, 0, 1, 0);
	up = XMVectorSet(0, 1, 0, 0);
	rotateX = 0;
	rotateY = 0;

}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::Update(float deltaTime)
{

	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(rotateY, rotateX, 0);
	XMVECTOR cameraDir = XMVector3Rotate(direction, rotQuat);
	XMMATRIX view = XMMatrixLookToLH(position, cameraDir, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
	if (GetAsyncKeyState('W') & 0x8000) {
		position += cameraDir * deltaTime;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		position -= cameraDir * deltaTime;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		position += XMVector3Cross(cameraDir, up) * deltaTime;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		position -= XMVector3Cross(cameraDir, up) * deltaTime;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		position += up * deltaTime;
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		position -= up * deltaTime;
	}

}

void Camera::Rotate(float x, float y)
{
	// Adjust the current rotation
	rotateX += x;
	rotateY += y;

	// Clamp the x between PI/2 and -PI/2
	//rotateX = max(min(rotateX, XM_PIDIV2), -XM_PIDIV2);

}


void Camera::SetProjectionMatrix(float aspectRatio)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)aspectRatio,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}


Camera::~Camera()
{
}

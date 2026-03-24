#pragma once

#include "Actor/CameraActor.h"

class CameraMovementActor : public Craft::CameraActor
{
public:
	CameraMovementActor();
	~CameraMovementActor() = default;

private:
	virtual void Tick(float deltaTime) override;

private:
	float moveSpeed = 1.0f;
	float dragSpeedOffset = 0.05f;

};


#pragma once

#include "Actor/Actor.h"

class QuadMeshActor : public Craft::Actor
{
public:
	QuadMeshActor();

	virtual void Tick(float deltaTime) override;
};


#pragma once

#include "SceneNode.h"

#include <stdio.h>

// Internal node
class SceneGroup : public SceneNode<SceneGroup>
{
public:
	void update()
	{
		printf(name);
	}
};
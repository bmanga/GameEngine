#pragma once

#include "SceneGroup.h"

// Level-of-detail node
class SwitchNode : public SceneGroup
{
public:
	void update()
	{
		printf(name);
	}
};
#pragma once

#include "SceneGroup.h"
#include "SceneNode.h"
#include "SwitchNode.h"

class SceneManager
{
public:
	SceneGroup root;
	SceneGroup group;
	SwitchNode node_a;
	SwitchNode node_b;

	SceneManager()
	{
		root.setName("Root\n");
		group.setName("Root/Group\n");
		node_a.setName("Root/Group/Node A\n");
		node_b.setName("Root/Group/Node B\n");

		// Note if the group node is modified after it is added
		// as a child to root, changes will not be visible when
		// traversing from root!
		// Pass by reference required!
		group.addChild(node_a);
		group.addChild(node_b);
		root.addChild(group);
	}
};
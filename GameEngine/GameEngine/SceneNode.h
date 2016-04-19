#pragma once

#include "math.h"
#include "RenderComponent.h"
#include "Mesh.h"
#include "material.h"

#include <vector>
#include <stdio.h>
#include <memory>

// Leaf node
template<class TDerivedNode>
class SceneNode
{
protected:
	const char* name;

	std::shared_ptr<SceneNode> parent;
	std::vector<std::shared_ptr<SceneNode>> children;

	glm::mat4 transform;
	glm::mat4 world_transform;

	std::shared_ptr<Lemur::Mesh> mesh;
	std::shared_ptr<Lemur::Material> material;

public:
	void setName(const char* name)
	{
		this->name = name;
	}

	template<class TChildNode>
	void addChild(TChildNode node)
	{
		static_assert(std::is_base_of<SceneNode<TDerivedNode>, TChildNode>::value, "");
		children.push_back(std::make_shared<TChildNode>(node));
	}

	void setTransform(glm::mat4 transform)
	{
		this->transform = transform;
	}

	void updateLocal()
	{
		if (parent)
		{
			// Child node
			world_transform = parent->world_transform * transform;
		}
		else
		{
			// Root node: transforms its local transform.
			world_transform = transform;
		}
		static_cast<TDerivedNode*>(this)->update();
	}

	void updateAll()
	{
		updateLocal();

		for (unsigned int i = 0; i < children.size(); i++)
		{
			children.at(i)->updateAll();
		}
	}

	const char* getName()
	{
		return name;
	}
};
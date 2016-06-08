#pragma once

#include "types.h"
#include <unordered_map>
#include <memory>

#include "texture.h"
#include "Mesh.h"

namespace Lemur
{

template <class Ty>
class ResourceManager
{
public:
	void unloadUnreferenced()
	{
		for (auto It = m_instances.begin(), End = m_instances.end(); It != End; ++It)
		{
			if (It->second.unique())
				m_instances.erase(It);
		}
	}

	template <class... Args>
	std::shared_ptr<Ty> load(const char* filename, Args&&... args)
	{
		auto ElemIt = m_instances.find(filename);
		if (ElemIt != m_instances.end())
		{
			return ElemIt->second;
		}

		auto ElemPtr = 
			std::make_shared<Ty>(filename, std::forward<Args>(args)...);
		m_instances.insert(std::pair<std::string, std::shared_ptr<Ty>>(filename, ElemPtr));
		return std::move(ElemPtr);
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Ty>> m_instances;
};

using MeshManager = ResourceManager<Mesh>;
using TextureManager = ResourceManager<Texture>;
}


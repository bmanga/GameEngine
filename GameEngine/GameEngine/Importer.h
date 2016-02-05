#pragma once

#include <tuple>
#include <vector>
#include <glm/detail/type_vec3.hpp>

template<class >class TaskFuture;

using ModelData = std::tuple<
	std::vector<glm::vec3>,
	std::vector<glm::vec2>,
	std::vector<glm::vec3 >> ;

void load_obj(const char* path);


#pragma once

#include <cstdint>
#include <tuple>
#include <vector>
#include <string>
#include "math.h"


namespace Lemur
{

template <class T>
using vector = std::vector<T>;
using std::string;
using  i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using  u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using MeshData = std::tuple <
	std::vector<glm::vec3>, //vector data
	std::vector<glm::vec2>, //texture data
	std::vector<glm::vec3 >,//normal data
	std::vector<Lemur::u32>,//vector indices
	std::vector<Lemur::u32>,//texture indices
	std::vector < Lemur::u32 >> ;//normal indices
}



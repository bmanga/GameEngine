#pragma once
#include <filesystem>

namespace Lemur {
namespace fs = std::experimental::filesystem;
static const fs::path MESH_PATH("..\\assets\\mesh");
static const fs::path TEXTURE_PATH("..\\assets\\textures");
static const fs::path SHADER_PATH("..\\assets\\shaders");
static const fs::path FONT_PATH("..\\assets\\fonts");
};
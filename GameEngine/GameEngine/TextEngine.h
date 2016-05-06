#pragma once

#include "FilePaths.h"
#include "freetype-gl-cpp/freetype-gl-cpp/TextureFont.h"
#include <unordered_map>

//TODO: string view is not implemented by vs yet. 
namespace
{
struct string_view
{
	const char* start;
	const size_t length;

	template<size_t N>
	string_view(const char(&str)[N]) : start(str), length(N)
	{
	
	}
};
}
namespace Lemur
{


/* TODO: right now each font corresponds to a separate atlas. We probably want to 
 * either let the user choose the atlas to write to, or create and algorithm
 * to do it automatically.
 */

class TextEngine
{
	static constexpr size_t FONT_NAME_LENGTH = 16;

public:
	bool loadFont(string_view name, float size, fs::path path);
	ftgl::Font* getFont(string_view name);

private:
	std::unordered_map<std::string, ftgl::Font> fonts;
	std::vector<std::unique_ptr<ftgl::TextureAtlas>> atlantes;

};
}
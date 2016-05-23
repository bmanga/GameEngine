#include "TextEngine.h"

bool Lemur::TextEngine::loadFont(string_view name, float size, fs::path path)
{
	return true;
	//atlantes.push_back(std::make_unique<ftgl::TextureAtlas>(512, 512, 1));
	//auto* atlasPtr = atlantes.back().get();

	//auto res = fonts.emplace(std::piecewise_construct,
	//	stdname.start, //key
	//	atlasPtr, ftgl::Font::File{ size, path.generic_string().c_str() } //value
	//);

	////return whether insertion was successful
	//return res.second;
}

ftgl::Font* Lemur::TextEngine::getFont(string_view name)
{
	return nullptr;
}

#include "TextEngine.h"
#include "OpenGLMaster.h"
#include "freetype-gl-cpp/freetype-gl-cpp/Utility.h"
#include <cassert>
#include <iterator>


bool Lemur::TextEngine::loadFont(std::string name, float size, fs::path path)
{
	atlantes.push_back(std::make_unique<ftgl::TextureAtlas>(512, 512, 1));
	auto* atlasPtr = atlantes.back().get();

	auto res = fonts.emplace( std::piecewise_construct,
		std::forward_as_tuple(std::move(name)), //key
		std::forward_as_tuple(atlasPtr, ftgl::Font::File{ size, path.generic_string().c_str() } )//value
	);

	//return whether insertion was successful
	return res.second;
}

ftgl::Font* Lemur::TextEngine::getFont(const std::string& name)
{
	auto font = fonts.find(name);
	return font != fonts.end() ?
		&(font->second) :
		nullptr;
}

void Lemur::TextEngine::displayText(const char* text, Pen& pen)
{
	auto* activeFont = getFont(pen.font);

	//make sure all the glyphs are actually loaded
	activeFont->loadGlyphs(text);

	const ftgl::Glyph* previous = nullptr;

	for (auto* glyph : ftgl::glyph_range(activeFont, text))
	{
		if (glyph == nullptr) continue;

		float kerning = previous ?
			glyph->getKerning(previous->codepoint) :
			0.0f;

		pen.x += kerning;

		int x0 = int(pen.x + glyph->offset_x);
		int y0 = int(pen.y + glyph->offset_y);
		int x1 = int(x0 + glyph->width);
		int y1 = int(y0 - glyph->height);
		float s0 = glyph->s0;
		float t0 = glyph->t0;
		float s1 = glyph->s1;
		float t1 = glyph->t1;


		GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };
		Vertex vertices[4] = {
			{float(x0), float(y0), 0,  s0, t0, pen.r, pen.g, pen.b, pen.a },
			{float(x0), float(y1), 0,  s0, t1, pen.r, pen.g, pen.b, pen.a },
			{float(x1), float(y1), 0,  s1, t1, pen.r, pen.g, pen.b, pen.a },
			{float(x1), float(y0), 0,  s1, t0, pen.r, pen.g, pen.b, pen.a }
		};

		buffer.push_back((const char*)vertices, 4, indices, 6);
		pen.x += glyph->advance_x;
		previous = glyph;

	}

}

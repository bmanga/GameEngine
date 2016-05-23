#include "TextEngine.h"
#include "OpenGLMaster.h"
#include "freetype-gl-cpp/freetype-gl-cpp/utf8Utils.h"
#include <cassert>

namespace
{

class glyph_iterator
{
private:
	size_t index = 0;
	const char* codepoints;
	ftgl::Font* font;

public:
	using underlying_type = const ftgl::Glyph*;

	glyph_iterator(ftgl::Font* font, const char* codepoints, size_t index) :
		font(font),
		codepoints(codepoints),
		index(index)
	{
		assert(codepoints);
	}

	glyph_iterator& operator++()
	{
		index += ftgl::utf8_surrogate_len(codepoints + index);
		return *this;
	}

	const ftgl::Glyph* operator*() const
	{
		return font->getLoadedGlyph(ftgl::utf8_to_utf32(codepoints + index));
	}

	bool operator!= (const glyph_iterator& itr) const
	{
		assert(itr.codepoints == codepoints && itr.font == font);

		if (itr.index != index) return true;

		return false;
	}
};

class glyph_range
{
private:
	const char* codepoints;
	ftgl::Font* font;
public:
	glyph_range(ftgl::Font* font, const char* codepoints) :
		codepoints(codepoints),
		font(font)
	{
		font->loadGlyphs(codepoints);
	}

	glyph_iterator begin() const 
	{
		return{ font, codepoints, 0 };
	}

	glyph_iterator end() const
	{
		return{ font, codepoints, ftgl::utf8_strlen(codepoints) };
	}

};

}
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

	for (auto* glyph : glyph_range(activeFont, text))
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
			{float(x0), float(y0), 0,  s0, t0},
			{float(x0), float(y1), 0,  s0, t1},
			{float(x1), float(y1), 0,  s1, t1},
			{float(x1), float(y0), 0,  s1, t0}
		};

		pen.x += glyph->advance_x;

		previous = glyph;

	}

}

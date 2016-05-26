#include "TextEngine.h"
#include "OpenGLMaster.h"
#include "freetype-gl-cpp/freetype-gl-cpp/Utility.h"
#include "FilePaths.h"
#include <cassert>
#include <iterator>


bool Lemur::TextEngine::loadFont(std::string name, float size, fs::path path)
{
	atlantes.push_back(std::make_unique<ftgl::TextureAtlas>(512, 512, 1));
	auto* atlasPtr = atlantes.back().get();

	auto res = fonts.emplace( std::piecewise_construct,
		std::forward_as_tuple(std::move(name)), //key
		std::forward_as_tuple(atlasPtr, size,
			ftgl::Font::File{ (FONT_PATH/path).generic_string().c_str() } )//value
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

void Lemur::TextEngine::addText(const char* text, Pen& pen)
{
	activeFont = getFont(pen.font);

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
		Color col = pen.color;
		Vertex vertices[4] = {
			{float(x0)/100, float(y0) / 100, 0,  s0, t0, col.r, col.g, col.b, col.a },
			{float(x0) / 100, float(y1) / 100, 0,  s0, t1, col.r, col.g, col.b, col.a },
			{float(x1) / 100, float(y1) / 100, 0,  s1, t1, col.r, col.g, col.b, col.a },
			{float(x1) / 100, float(y0) / 100, 0,  s1, t0, col.r, col.g, col.b, col.a }
		};

		buffer.push_back((const char*)vertices, 4, indices, 6);
		pen.x += glyph->advance_x;
		previous = glyph;

	}
	activeFont->atlas()->upload();

}

void Lemur::TextEngine::addText(std::string text, Pen& pen)
{
	addText(text.c_str(), pen);
}

void Lemur::TextEngine::display(lm::mat4 view, lm::mat4 proj)
{
	if (!activeFont) return;

	glBindTexture(GL_TEXTURE_2D, activeFont->atlas()->id());

	//glClearColor(1, 1, 1, 1);

	glEnable(GL_BLEND);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	program.use();

	auto model = glm::mat4();

	auto model_uniform = program.getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, 0, glm::value_ptr(model));

	auto view_uniform = program.getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, 0, glm::value_ptr(view));

	auto proj_uniform = program.getUniformLocation("projection");
	glUniformMatrix4fv(proj_uniform, 1, 0, glm::value_ptr(proj));

	buffer.render(GL_TRIANGLES);

	glDisable(GL_BLEND);
}

#include "TextEngine.h"
#include "OpenGLMaster.h"
#include "freetype-gl-cpp/freetype-gl-cpp/Utility.h"
#include "FilePaths.h"
#include <cassert>
#include <iterator>
#include "Math.h"
#include "ConsoleLogger.h"
#include "Lemur.h"


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

	if(!activeFont)
	{
		Lemur::ConsoleLogger::Error(CODE_LOCATION, "attempting to use a font"
		" which has not been loaded");
		return;
	}

	const ftgl::Glyph* previous = nullptr;

	for (auto* glyph : ftgl::glyph_range(activeFont, text))
	{
		if (glyph == nullptr) continue;

		float kerning = previous ?
			glyph->getKerning(previous->codepoint) :
			0.0f;

		pen.x += kerning;

		//TODO(bmanga): get rid of the hardcoded screen size
		float x0 = (pen.x + glyph->offset_x) / 400 - 1.f;
		float y0 = (pen.y + glyph->offset_y - activeFont->height()) / 300 + 1.f;
		float w = glyph->width / 400.f;
		float h = glyph->height / 300.f;
		float x1 = x0 + w;
		float y1 = y0 - h;

		float s0 = glyph->s0;
		float t0 = glyph->t0;
		float s1 = glyph->s1;
		float t1 = glyph->t1;


		GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };
		Color col = pen.color;
		Vertex vertices[4] {
			{x0, y0, 0,  s0, t0, col.r, col.g, col.b, col.a },
			{x0, y1, 0,  s0, t1, col.r, col.g, col.b, col.a },
			{x1, y1, 0,  s1, t1, col.r, col.g, col.b, col.a },
			{x1, y0, 0,  s1, t0, col.r, col.g, col.b, col.a }
		};
		pen.x += glyph->advance_x;

		buffer.push_back((const char*)vertices, 4, indices, 6);
		previous = glyph;
	}
	activeFont->atlas()->upload();

}

void Lemur::TextEngine::addText(std::string text, Pen& pen)
{
	addText(text.c_str(), pen);
}

void Lemur::TextEngine::display()
{
	if (!activeFont) return;

	//glm::lookAt(glm::vec3{ 0, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 });

	glBindTexture(GL_TEXTURE_2D, activeFont->atlas()->id());

	//glClearColor(1, 1, 1, 1);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	program.use();

	auto model = glm::mat4();
	auto view  = glm::mat4();
	auto proj  = glm::mat4();
	//model = glm::rotate(model, 90.0f, glm::vec3{ 1, 0, 0 });

	auto model_uniform = program.getUniformLocation("model");
	glUniformMatrix4fv(model_uniform, 1, 0, glm::value_ptr(model));

	auto view_uniform = program.getUniformLocation("view");
	glUniformMatrix4fv(view_uniform, 1, 0, glm::value_ptr(view));

	auto proj_uniform = program.getUniformLocation("projection");
	glUniformMatrix4fv(proj_uniform, 1, 0, glm::value_ptr(proj));

	buffer.render(GL_TRIANGLES);

	glDisable(GL_BLEND);
	
}

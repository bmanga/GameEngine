#pragma once

#include "FilePaths.h"
#include "freetype-gl-cpp/freetype-gl-cpp/TextureFont.h"
#include <unordered_map>
#include <string>
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "math.h"
namespace Lemur
{

struct Vertex
{
	float x, y, z;    // position
	float s, t;       // texture
	float r, g, b, a; // color
};

struct Color
{
	float r, g, b, a;
};
struct Pen
{
	float x, y;
	Color color;
	std::string font;
};

/* TODO: right now each font corresponds to a separate atlas. We probably want to 
 * either let the user choose the atlas to write to, or create and algorithm
 * to do it automatically.
 */
namespace lm = Lemur::math;
class TextEngine
{
public:
	TextEngine():
		buffer("vertex:3f,tex_coord:2f,color:4f"),
		program("v3f-t2f-c4f.vert", "v3f-t2f-c4f.frag")
	{
		program.compile();
	}

	bool loadFont(std::string name, float size, fs::path path);
	ftgl::Font* getFont(const std::string& name);
	void addText(const char* text, Pen& pen);
	void addText(std::string text, Pen& pen);
	void display();
	void clear()
	{
		buffer.clear();
	}

private:
	VertexBuffer buffer;
	ShaderProgram program;
	ftgl::Font* activeFont = nullptr;
	std::unordered_map<std::string, ftgl::Font> fonts;
	std::vector<std::unique_ptr<ftgl::TextureAtlas>> atlantes;

};
}

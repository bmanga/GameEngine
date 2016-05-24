#pragma once

#include "FilePaths.h"
#include "freetype-gl-cpp/freetype-gl-cpp/TextureFont.h"
#include <unordered_map>
#include <string>
#include "VertexBuffer.h"
#include "ShaderProgram.h"

namespace Lemur
{

struct Vertex
{
	float x, y, z;    // position
	float s, t;       // texture
	float r, g, b, a; // color
};

struct Pen
{
	float x, y;
	float r, g, b, a;
	std::string font;
};

/* TODO: right now each font corresponds to a separate atlas. We probably want to 
 * either let the user choose the atlas to write to, or create and algorithm
 * to do it automatically.
 */

class TextEngine
{
public:
	TextEngine():
		buffer("vertex:3f,tex_coord:2f,color:4f"),
		program("v3f-t2f-c4f.vert", "v3f-t2f-c4f.frag")
	{}

	bool loadFont(std::string name, float size, fs::path path);
	ftgl::Font* getFont(const std::string& name);
	void displayText(const char* text, Pen& pen);

private:
	VertexBuffer buffer;
	ShaderProgram program;
	std::unordered_map<std::string, ftgl::Font> fonts;
	std::vector<std::unique_ptr<ftgl::TextureAtlas>> atlantes;

};
}

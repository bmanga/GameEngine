#pragma once

class Shader
{
private:
	unsigned int id;
	int type;
	const char* source;

	void create();
	bool compile();

	void printLog();

public:
	Shader(int type, const char* source);

	int getType();
	unsigned int getId();
};
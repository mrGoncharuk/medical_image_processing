#pragma once

#include <string>

#include <iostream>
#include <vector>
#include <map>

#include "Graphics.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class Text
{
private:
	GLuint		vao;
	GLuint		vbo;
	// GLuint		programID;
    Shader      shader;
	int			windowWidth;
	int			windowHeight;
	std::map<GLchar, Character> Characters;
	glm::mat4	MVP;

public:
	Text(/* args */);
	Text(int winWidth, int winHeight);
	~Text();

	void		createShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
	void		loadFont(const std::string &fontPath);
	void 		renderText(std::string text, float x, float y, float z, float scale, glm::vec3 color);
	void		setMVP(glm::mat4 mvp);
};

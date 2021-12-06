#pragma once

#include <iostream>
#include <string>
#include <map>

#include "Graphics.hpp"
#include <glad/glad.h>


class Texture
{
private:
	GLuint id;
	int width;
	int height;

public:

	Texture(char *imageData, int width, int heigth, std::map<std::string, int> params)
	{
		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_2D, this->id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (imageData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, params["internalFormat"], width, heigth, 0, params["format"], params["type"], imageData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "ERROR::TEXTURE::TEXTURE_LOADING_FAILED\n";
		}
    	// glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	~Texture()
	{
		glDeleteTextures(1, &this->id);
	}

	inline GLuint getID() const { return this->id; }


	void reloadFromData(char *imageData, int width, int heigth, std::map<std::string, int> params)
	{
		glBindTexture(GL_TEXTURE_2D, this->id);

		if (imageData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, params["internalFormat"], width, heigth, 0, params["format"], params["type"], imageData);
		}
		else
		{
			std::cout << "ERROR::TEXTURE::TEXTURE_LOADING_FAILED\n";
		}
    	// glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void bind(const GLint texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_2D, this->id);
	}

	void unbind()
	{
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

};
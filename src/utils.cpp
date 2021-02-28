#include "utils.hpp"



void read_shader_src(const char *fname, std::vector<char> &buffer) {
	std::ifstream in;
	in.open(fname, std::ios::binary);

	if(in.is_open()) {
		// Get the number of bytes stored in this file
		in.seekg(0, std::ios::end);
		size_t length = (size_t)in.tellg();

		// Go to start of the file
		in.seekg(0, std::ios::beg);

		// Read the content of the file in a buffer
		buffer.resize(length + 1);
		in.read(&buffer[0], length);
		in.close();
		// Add a valid C - string end
		buffer[length] = '\0';
	}
	else {
		std::cerr << "Unable to open " << fname << " I'm out!" << std::endl;
		exit(-1);
	}
}

// Compile a shader
GLuint load_and_compile_shader(const char *fname, GLenum shaderType) {
	// Load a shader from an external file
	std::vector<char> buffer;
	read_shader_src(fname, buffer);
	const char *src = &buffer[0];

	// Compile the shader
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	// Check the result of the compilation
	GLint test;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if(!test) {
		std::cerr << "Shader compilation failed with this message:" << std::endl;
		std::vector<char> compilation_log(512);
		glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
		std::cerr << &compilation_log[0] << std::endl;
		glfwTerminate();
		exit(-1);
	}
	return shader;
}

// Create a program from two shaders
GLuint create_program(const char *path_vert_shader, const char *path_frag_shader) {
	// Load and compile the vertex and fragment shaders
	GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
	GLuint fragmentShader = load_and_compile_shader(path_frag_shader, GL_FRAGMENT_SHADER);

	// Attach the above shader to a program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Flag the shaders for deletion
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Link and use the program
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	return shaderProgram;
}


GLfloat *normalize_coordinates(const std::vector<float> &buf)
{
    GLfloat *vertices_position = new GLfloat[buf.size()];

    for (int i = 0; i < buf.size(); i += 2)
    {
        vertices_position[i] = buf[i] / SCREEN_WIDTH;
        vertices_position[i + 1] = buf[i + 1] / SCREEN_HEIGHT;

    }
    return vertices_position;
}


unsigned char	*generateLUT(bool isStandart)
{
	const int byteLength = 256;
	static unsigned char LUT[byteLength];

	if (isStandart)
	{
		for (int i = 0; i < byteLength; i++)
			LUT[i] = i;
	}
	else
	{
		for (int i = 0; i < byteLength; i++)
			LUT[i] = byteLength - (i + 1);
	}
	
	return LUT;
}

void	applyLUTRedChannel(unsigned char *image_data, unsigned char *LUT, const int image_width, const int image_height, const int channels)
{
	for (int i = 0; i < image_height; i++)
		for (int j = 0; j < image_width; j++)
		{
			image_data[(i * image_width + j) * channels + 2] = 0;
			image_data[(i * image_width + j) * channels + 1] = 0;
			image_data[(i * image_width + j) * channels + 0] = LUT[image_data[(i * image_width + j) * 3]];
		}
}


unsigned char	*generateMask(const int image_width, const int image_height, int type)
{
	unsigned char *mask = new unsigned char[image_width * image_height];

	if (type == 0)
	{
		for (int i = 0; i < image_height; i++)
			for (int j = 0; j < image_width; j++)
				if (j >= image_width / 2)
					mask[i * image_width + j] = 0;
				else
					mask[i * image_width + j] = 1;
	}
	else if (type == 1)
	{
		for (int i = 0; i < image_height; i++)
			for (int j = 0; j < image_width; j++)
				if (j < image_width / 2)
					mask[i * image_width + j] = 0;
				else
					mask[i * image_width + j] = 1;
	}
	else if (type == 2)
	{
		for (int i = 0; i < image_height; i++)
			for (int j = 0; j < image_width; j++)
				if (i >= image_height / 2)
					mask[i * image_width + j] = 0;
				else
					mask[i * image_width + j] = 1;
	}
	else if (type == 3)
	{
		for (int i = 0; i < image_height; i++)
			for (int j = 0; j < image_width; j++)
				if (i < image_height / 2)
					mask[i * image_width + j] = 0;
				else
					mask[i * image_width + j] = 1;
	}
	return mask;
}


void	applyMask(unsigned char *image_data, unsigned char *mask, const int image_width, const int image_height, const int channels)
{
	for (int i = 0; i < image_height; i++)
		for (int j = 0; j < image_width; j++)
			if ( !mask[i * image_width + j] )
				for (int c = 0; c < channels; c++)
					image_data[(i * image_width + j) * channels + c] = 0;
	
}
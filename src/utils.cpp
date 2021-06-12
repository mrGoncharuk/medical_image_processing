#include "utils.hpp"
#include <vector>
using namespace std;


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

void	countHistogram(const unsigned short *image_data, int length, int channels, float hist[], float *maxVal)
{
	*maxVal = 0;
	memset(hist, 0, sizeof(float) * USHRT_MAX + 1);

	for(int i=0; i < length; i += channels)
		hist[image_data[i]] += + 1.0f;
	hist[0] = 0;
	for(int i=0; i < USHRT_MAX + 1; i++)
		*maxVal = MAX(hist[i], *maxVal);

}

void initSingleChannelImage(unsigned short *input, unsigned short *output, int length, int channels)
{
	for (int i = 0; i < length; i++)
	{
		output[i] = input[i*channels];
	}
}

void initMultiChannelImage(unsigned short *input, unsigned short *output, int length, int channels)
{
	for (int i = 0; i < length; i++)
	{
		for (int channel = 0; channel < channels; channel++)
			input[i*channels + channel] = output[i];
	}
}

void equalizeHistogram(unsigned short* pdata, int width, int height, int channels, int max_val = 255)
{
    int total = width*height*channels;
    int n_bins = max_val + 1;

    // Compute histogram
    vector<int> hist(n_bins, 0);
    for (int i = 0; i < total; i += channels) {
        hist[pdata[i]]++;
    }

    // Build LUT from cumulative histrogram

    // Find first non-zero bin
    int i = 0;
    while (!hist[i]) ++i;

    if (hist[i] == total/channels) {
        for (int j = 0; j < total; j += channels) { 
            for (int c = 0; c < channels; c++)
				pdata[j + c] = i; 
        }
        return;
    }

    // Compute scale
    float scale = (n_bins - 1.f) / ((total/channels) - hist[i]);
    // Initialize lut
    vector<int> lut(n_bins, 0);
    i++;

    int sum = 0;
    for (; i < hist.size(); ++i) {
        sum += hist[i];
        // the value is saturated in range [0, max_val]
        lut[i] = max(0, min(int(round(sum * scale)), max_val));
    }

    // Apply equalization
    for (int i = 0; i < total; i += channels) {
		for (int c = 0; c < channels; c++)
        	pdata[c + i] = lut[pdata[c + i]];
    }
}

void	applyMask(unsigned short *image_data, unsigned short *mask, const int image_width, const int image_height, const int channels)
{
	for (int i = 0; i < image_height; i++)
		for (int j = 0; j < image_width; j++)
			if ( !mask[i * image_width + j] )
				for (int c = 0; c < channels; c++)
					image_data[(i * image_width + j) * channels + c] = 0;
	
}

void	applyColorMask(unsigned short *image_data, unsigned short *mask, const int image_width, const int image_height, const int channels)
{
	for (int i = 0; i < image_height; i++)
		for (int j = 0; j < image_width; j++)
			if ( mask[i * image_width + j] )
				for (int c = 1; c < channels; c++)
					image_data[(i * image_width + j) * channels + c] = 0;

}


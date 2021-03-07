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

void	countHistogram(const unsigned char *image_data, int length, int channels, float hist[], float *maxVal)
{
	*maxVal = 0;
	memset(hist, 0, sizeof(float) * 256);

	for(int i=0; i < length; i += channels)
		hist[image_data[i]] += + 1.0f;
	hist[0] = 0;
	for(int i=0; i < 256; i++)
		*maxVal = MAX(hist[i], *maxVal);
}

unsigned char	*histogramEqualisation(unsigned char* image_data, int cols, int rows, int channels) 
{ 
    // creating image pointer 
    unsigned char* image; 
  
    // Declaring 2 arrays for storing histogram values (frequencies) and 
    // new gray level values (newly mapped pixel values as per algorithm) 
    int hist[256] = { 0 }; 
    int new_gray_level[256] = { 0 }; 
  
    // Declaring other important variables 
    int col, row, total, curr; 
  
    // allocating image array the size equivalent to number of columns 
    // of the image to read one row of an image at a time 

	// calculating total number of pixels 
    total = cols * rows; 
	// allocating memory for new image
    image = new unsigned char[total * channels];
  
  
    // Calculating frequency of occurrence for all pixel values 

	for(int i = 0; i < total * channels; i += channels)
		hist[(int)image_data[i]]++;
  
    
  
    curr = 0; 
  
    // calculating cumulative frequency and new gray levels 
    for (int i = 0; i < 256; i++) { 
        // cumulative frequency 
        curr += hist[i]; 
  
        // calculating new gray level after multiplying by 
        // maximum gray count which is 255 and dividing by 
        // total number of pixels 
        new_gray_level[i] = round((((float)curr) * 255) / total); 
    }

    // performing histogram equalisation by mapping new gray levels 
	for(int i = 0; i < total; i++)
	{
		for (int c = 0; c < channels; c++)
			image[i * channels + c] = (unsigned char)new_gray_level[image_data[i * channels + c]];
	}
	for (int i = 0; i < total * channels; i++)
		image_data[i] = image[i];
	
	delete[] image;
	return (image_data);
} 


void	peakNormalization(unsigned char* image_data, int width, int heigth, int channels, int newMin, int newMax, int peakRange[2])
{
	int hist[256] = { 0 };
	int peakMax = peakRange[0];
	int peakMin = peakRange[0];
	int total;

	total = width * heigth;

	for(int i = 0; i < total * channels; i += channels)
		hist[(int)image_data[i]]++;

	for(int i = peakRange[0]; i < peakRange[1]; i++)
	{
		if (hist[i] < hist[peakMax])
			peakMax = i;
		if (hist[i] > hist[peakMin])
			peakMin = i;
	}
	std::cout << peakMin << " " << peakMax << std::endl;
	float tmp;
	for (int i = 0; i < total; i++)
	{
		float tmp = newMin + (image_data[i * channels] - peakMin) / (float)(peakMax - peakMin) * (newMax - newMin);
		
		if (tmp > 255)
			tmp = 255;
		// std::cout << tmp << std::endl;
		image_data[i * channels] = (unsigned char)round(tmp);
		image_data[i * channels + 1] = (unsigned char)round(tmp);
		image_data[i * channels + 2] = (unsigned char)round(tmp);
	}
}

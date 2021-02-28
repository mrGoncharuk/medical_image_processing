#include "ImageRenderer.hpp"


ImageRenderer::ImageRenderer(/* args */): vbo(0), vao(0), programID(0), textureID(0), image_data(NULL)
{
}


ImageRenderer::~ImageRenderer()
{
}


void		ImageRenderer::createShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
{
	// Load and compile the vertex and fragment shaders
	GLuint vertexShader = load_and_compile_shader(vertexShaderPath.c_str(), GL_VERTEX_SHADER);
	GLuint fragmentShader = load_and_compile_shader(fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER);

	// Attach the above shader to a program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Flag the shaders for deletion
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Link the program
	glLinkProgram(shaderProgram);
	// glUseProgram(shaderProgram);

	this->programID = shaderProgram;
}

	
void		ImageRenderer::loadImage(const std::string &imagePath)
{
    	// Use a Vertex Array Object
	glGenVertexArrays(1, &(this->vao));
	glBindVertexArray(this->vao);

	// 1 square (made by 2 triangles) to be rendered
	GLfloat vertices_position[8] = {
		-1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		-1.0, 1.0,
				
	};

 	GLfloat texture_coord[8] = {
 		
 		0.0, 1.0,
 		1.0, 1.0,
 		1.0, 0.0,
		 0.0, 0.0,
	};

	GLuint indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	// Create a Vector Buffer Object that will store the vertices on video memory
	glGenBuffers(1, &vbo);

	// Allocate space for vertex positions and texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_position) + sizeof(texture_coord), NULL, GL_STATIC_DRAW);

	// Transfer the vertex positions:
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_position), vertices_position);

	// Transfer the texture coordinates:
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_position), sizeof(texture_coord), texture_coord);

	// Create an Element Array Buffer that will store the indices array:
	GLuint eab;
	glGenBuffers(1, &eab);

	// Transfer the data from indices to eab
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Create a texture
	glGenTextures(1, &(this->textureID));

	// Specify that we work with a 2D texture
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	ReadDicomImage(imagePath.c_str());

	// this->programID = create_program("shaders/vert.shader", "shaders/frag.shader");
    glUseProgram(this->programID);
	// Get the location of the attributes that enters in the vertex shader
	GLint position_attribute = glGetAttribLocation(this->programID, "position");

	// Specify how the data for position can be accessed
	glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable the attribute
	glEnableVertexAttribArray(position_attribute);

	// Texture coord attribute
	GLint texture_coord_attribute = glGetAttribLocation(this->programID, "texture_coord");
	glVertexAttribPointer(texture_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(vertices_position));
	glEnableVertexAttribArray(texture_coord_attribute);
}


void		ImageRenderer::ReadDicomImage(const std::string &fname)
{
	char 				*image_data_raw = NULL;
	unsigned char		*image_data_v = NULL;
	imebra::DataSet 	loadedDataSet(imebra::CodecFactory::load(fname));
	imebra::Image		image(loadedDataSet.getImage(0));
	std::uint32_t 		width = image.getWidth();
	std::uint32_t 		height = image.getHeight();

	std::cout << "width: " << width << " height: " << height << std::endl;
	if (loadedDataSet.getString(imebra::TagId(0x0028, 0x1052), 0) != "0"
		&& loadedDataSet.getString(imebra::TagId(0x0028, 0x1053), 0) != "1")
	{
		std::cout << "Processing image with pixels in float is not yet implemented. Exiting." << std::endl;
		exit(1);
	}
	image_data_raw = new char[width * height];
	image.getReadingDataHandler().data(image_data_raw, width * height);

	image_data_v = new unsigned char[width * height * 3];
	for (size_t i = 0; i < width * height; i++)
	{
		image_data_v[i * 3] = image_data_raw[i];
		image_data_v[i * 3 + 1] = image_data_raw[i];
		image_data_v[i * 3 + 2] = image_data_raw[i];
	}
	this->channels = 3;
	this->width = width;
	this->height = height;
	this->image_data_original = image_data_v;
	this->image_data = new unsigned char[width * height * this->channels];
	std::copy(this->image_data_original, this->image_data_original + this->width * this->height * this->channels, this->image_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	delete []image_data_raw;
}


void	ImageRenderer::restoreImageData()
{
	std::copy(this->image_data_original, this->image_data_original + this->width * this->height * this->channels, this->image_data);
}


void	ImageRenderer::redrawImage()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void 		ImageRenderer::renderImage()
{
    glUseProgram(this->programID);
    // glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->vao);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

unsigned char	*ImageRenderer::getImageData() const { return (this->image_data); }
int		ImageRenderer::getChannels() const { return (this->channels); }
int		ImageRenderer::getWidth() const { return (this->width); }
int		ImageRenderer::getHeight() const { return (this->height); }




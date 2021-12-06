#include "MIA.hpp"


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


//Private functions
void MIA::initGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);
	if (glfwInit() == GLFW_FALSE)
	{
		std::cerr << "ERROR: Failed initialization of GLFW" << std::endl;
		glfwTerminate();
	}
}

void MIA::initWindow(const char* title, bool resizable)
{
 // Decide GL+GLSL versions
#ifdef __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, title, NULL, NULL);
    if (this->window == nullptr)
    {
        std::cerr << "ERROR: Failed creation of window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // Enable vsync
}

void MIA::initGLAD()
{
 // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        std::cerr << "Failed to initialize OpenGL loader" << std::endl;
        glfwTerminate();
    }
}

void MIA::initOpenGLOptions()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    
}

void MIA::initImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    // ImGui_ImplOpenGL3_Init(glsl_version);
    const char* glsl_version = "#version 330 core";

    ImGui_ImplOpenGL3_Init(glsl_version);

}


void MIA::initShaders()
{
	this->shaders.push_back(new Shader("shaders/texture.vs", "shaders/texture.fs"));
	this->shaders.push_back(new Shader("shaders/rectangle.vs", "shaders/rectangle.fs"));

}

void MIA::initTextures()
{
    std::string path = "/home/mhoncharuk/Education/MIA/data/DICOM_set_16bits";
	dct.readDicomSet(path);
	ss.init((short **)dct.getImageSet(), dct.params["width"], dct.params["heigth"], dct.params["length"]);
	this->textures.push_back(new Texture((char *)ss.getTransverseSlice(0), dct.params["width"], dct.params["heigth"], dct.params));
	this->textures.push_back(new Texture((char *)ss.getFrontalSlice(0), dct.params["width"], dct.params["length"], dct.params));
	this->textures.push_back(new Texture((char *)ss.getSagitalSlice(0), dct.params["heigth"], dct.params["length"], dct.params));
}



void MIA::initMeshes()
{
	GLuint indices[6] = {
		0, 1, 3,
		1, 2, 3
	};
    float vertices[] = {
			//Position								//Color							//Texcoords					

            -1.0f, 0.0f, 0.f,			1.f, 0.f, 0.f,		0.0f, 1.0f,
			0.0f, 0.0f, 0.f,			    0.f, 1.f, 0.f,		1.0f, 1.0f,
			0.0f, 1.0f, 0.f,			    0.f, 0.f, 1.f,		1.0f, 0.0f,
			-1.0f, 1.0f, 0.f,			1.f, 1.f, 0.f,		0.0f, 0.0f
                        };

    meshes.push_back(new Mesh(vertices, 4, indices, 6));

    float vertices2[] = {
			//Position								//Color							//Texcoords					

            0.0f, 0.0f, 0.f,			1.f, 0.f, 0.f,		0.0f, 1.0f,
			1.0f, 0.0f, 0.f,			    0.f, 1.f, 0.f,		1.0f, 1.0f,
			1.0f, 1.0f, 0.f,			    0.f, 0.f, 1.f,		1.0f, 0.0f,
			0.0f, 1.0f, 0.f,			1.f, 1.f, 0.f,		0.0f, 0.0f
                        };
    meshes.push_back(new Mesh(vertices2, 4, indices, 6));


    float vertices3[] = {
			//Position								//Color							//Texcoords					

            -1.0f, -1.0f, 0.f,			1.f, 0.f, 0.f,		0.0f, 1.0f,
			0.0f, -1.0f, 0.f,			    0.f, 1.f, 0.f,		1.0f, 1.0f,
			0.0f, 0.0f, 0.f,			    0.f, 0.f, 1.f,		1.0f, 0.0f,
			-1.0f, 0.0f, 0.f,			1.f, 1.f, 0.f,		0.0f, 0.0f
                        };
    meshes.push_back(new Mesh(vertices3, 4, indices, 6));

    float vertices4[] = {
			//Position								//Color							//Texcoords					

            0.0f, 0.0f, 0.0f,			1.f, 0.f, 0.f,		0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,			    1.f, 0.f, 0.f,		0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,			    1.f, 0.f, 0.f,		0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,			1.f, 0.f, 0.f,		0.0f, 0.0f
                        };
    meshes.push_back(new Mesh(vertices4, 4, indices, 6));
	float vertices5[] = {
		//Position								//Color							//Texcoords					

		0.0f, 0.0f, 0.0f,			0.f, 1.f, 0.f,		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,			    0.f, 1.f, 0.f,		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,			    0.f, 1.f, 0.f,		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,			0.f, 1.f, 0.f,		0.0f, 0.0f
					};
meshes.push_back(new Mesh(vertices5, 4, indices, 6));
	
}

//Constructors / Destructors
MIA::MIA(
	const char* title,
	const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
	bool resizable
)
	:
	WINDOW_WIDTH(WINDOW_WIDTH),
	WINDOW_HEIGHT(WINDOW_HEIGHT),
	rulerState(0)
{
	//Init variables
	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->WINDOW_HEIGHT;

	this->dt = 0.f;
	this->curTime = 0.f;
	this->lastTime = 0.f;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;

	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLAD();
	this->initOpenGLOptions();
    this->initImGui();

	this->initShaders();
	this->initTextures();
	this->initMeshes();
	this->rulerLine = new Line(glm::vec3(0), glm::vec3(0));
}

MIA::~MIA()
{
	for (size_t i = 0; i < this->shaders.size(); i++)
		delete this->shaders[i];
	
	for (size_t i = 0; i < this->textures.size(); i++)
		delete this->textures[i];

    for (size_t i = 0; i < this->meshes.size(); i++)
		delete this->meshes[i];

	delete this->rulerLine;
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

//Accessor
int MIA::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

//Modifier
void MIA::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

//Functions
void MIA::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

void MIA::updateMouseInput()
{
	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && rulerState != 0)
	{
		switch (this->rulerState)
		{
		case 1:
			glfwGetCursorPos(this->window, &this->lastMouseX, &this->lastMouseY);
			this->meshes[3]->resetPosition(2 * this->lastMouseX / this->WINDOW_WIDTH - 1, 1 - 2 * this->lastMouseY / this->WINDOW_HEIGHT);
			break;
		case 2:
			glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);
			this->meshes[4]->resetPosition(2 * this->mouseX / this->WINDOW_WIDTH - 1, 1 - 2 * this->mouseY / this->WINDOW_HEIGHT);

			break;
		default:
			break;

		}
		rulerLine->setPosition(glm::vec3(2 * this->lastMouseX / this->WINDOW_WIDTH - 1,  1 - 2 * this->lastMouseY / this->WINDOW_HEIGHT, 0),
								glm::vec3(2 * this->mouseX / this->WINDOW_WIDTH - 1, 1 - 2 * this->mouseY / this->WINDOW_HEIGHT, 0));
	}

}

void MIA::updateKeyboardInput()
{
	//Program
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->window, GLFW_TRUE);
	}

}

void MIA::updateInput()
{
	glfwPollEvents();
	
	this->updateKeyboardInput();

	this->updateMouseInput();
}

void MIA::update()
{
	this->updateDt();
	this->updateInput();

    // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    // static bool show_demo_window = true;
	// if (show_demo_window)
    //     ImGui::ShowDemoWindow(&show_demo_window);
	static int	horSlice = 0;
	static int	frontSlice = 0;
	static int	sagSlice = 0;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	{
        ImGui::Begin("Image Slicing");
		ImGui::PushItemWidth(100);
        if (ImGui::SliderInt("Transverse slice", &horSlice, 0, 19))
            textures[0]->reloadFromData((char *)ss.getTransverseSlice(horSlice), dct.params["width"], dct.params["heigth"], dct.params);
        if (ImGui::SliderInt("Frontal slice", &frontSlice, 0, 255))
            textures[1]->reloadFromData((char *)ss.getFrontalSlice(frontSlice), dct.params["width"], dct.params["length"], dct.params);
		if (ImGui::SliderInt("Sagital slice", &sagSlice, 0, 255))
            textures[2]->reloadFromData((char *)ss.getSagitalSlice(sagSlice), dct.params["heigth"], dct.params["length"], dct.params);

        ImGui::RadioButton("Disable ruler", &this->rulerState, 0);
        ImGui::RadioButton("Setting first point", &this->rulerState, 1);
        ImGui::RadioButton("Setting second point", &this->rulerState, 2);
		if (this->rulerState)
		{
			double pixel_dist = sqrt(pow(this->mouseX - this->lastMouseX, 2) + pow(this->mouseY - this->lastMouseY, 2) * 1.0);
			double real_dist = sqrt(pow((this->mouseX - this->lastMouseX) * dct.getPixelSpacingX(), 2) + pow((this->mouseY - this->lastMouseY) * dct.getPixelSpacingY(), 2) * 1.0);

			ImGui::Text("Pixel Distance: %.2f", pixel_dist);
			ImGui::Text("Real Distance: %.2fmm", real_dist); 
			ImGui::Text("Point1(%.2f; %.2f);", this->mouseX, this->mouseY);
			ImGui::Text("Point2(%.2f; %.2f);", this->lastMouseX, this->lastMouseY);
		}
        ImGui::End();
    }

}

void MIA::render()
{
	//UPDATE --- 
	// updateInput(window);
    ImGui::Render();

	//DRAW ---
	//Clear
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //Use a program (BECAUSE SHADER CLASS LAST UNIFORM UPDATE UNUSES IT)
	textures[0]->bind(0);
	meshes[0]->render(shaders[0]);
	textures[1]->bind(0);
	meshes[1]->render(shaders[0]);
	textures[2]->bind(0);
	meshes[2]->render(shaders[0]);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (this->rulerState)
	{
		meshes[3]->render(shaders[1]);
		meshes[4]->render(shaders[1]);
		rulerLine->draw();
	}
	//End Draw
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	// glFlush();

	// glBindVertexArray(0);
	// glUseProgram(0);

}

//Static functions
void MIA::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
};



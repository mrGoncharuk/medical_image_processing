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
    // glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enable blending for text rendering
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
	shaders.push_back(new Shader("shaders/line.vs", "shaders/line.fs"));
	shaders.push_back(new Shader("shaders/point.vs", "shaders/point.fs"));

}

void MIA::initSceneObjects()
{

	points.push_back(new Point(glm::vec3(0.0f, 0.0f, 0.0f)));	// First camera
	points.push_back(new Point(glm::vec3(-540.0f, 0.0f, 0.0f)));	// Second camera
	points.push_back(new Point(glm::vec3(0.0f, 0.0f, 3000.0f), false));	// First camera projection point
	points.push_back(new Point(glm::vec3(-540.0f, 0.0f, 3000.0f), false));	// Second camera projection point
	points.push_back(new Point(glm::vec3(0.0f, 0.0f, 0.0f), false));	// point to find


	points[0]->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
	points[1]->setColor(glm::vec3(0.0f, 0.0f, 1.0f));
	points[2]->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
	points[3]->setColor(glm::vec3(0.0f, 0.5f, 1.0f));
	points[4]->setColor(glm::vec3(0.0f, 1.0f, 0.0f));


	lines.push_back(new Line(glm::vec3(-4500.0f, 0.0f, 0.0f), glm::vec3(4500.0f, 0.0f, 0.0f)));	// X Axis line
	lines.push_back(new Line(glm::vec3(0.0f, -4500.0f, 0.0f), glm::vec3(0.0f, 4500.0f, 0.0f))); // Y Axis line
	lines.push_back(new Line(glm::vec3(0.0f, 0.0f, -4500.0f), glm::vec3(0.0f, 0.0f, 4500.0f))); // Z Axis line
	lines.push_back(new Line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));		// Camera 1 to point
	lines.push_back(new Line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)));		// Camera 2 to point


	lines[0]->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
	lines[1]->setColor(glm::vec3(0.0f, 1.0f, 0.0f));
	lines[2]->setColor(glm::vec3(0.0f, 0.0f, 1.0f));
	lines[3]->setColor(glm::vec3(0.0f, 1.0f, 1.0f));
	lines[4]->setColor(glm::vec3(0.0f, 1.0f, 1.0f));


	glm::mat4 orto_proj = glm::ortho(-4000.0f, 4000.0f, -4000.0f, 4000.0f, -4000.0f, 4000.0f);
	for (size_t i = 0; i < points.size(); i++)
		points[i]->setMVP(orto_proj);
	for (size_t i = 0; i < lines.size(); i++)
		lines[i]->setMVP(orto_proj);

	texts.push_back(new Text(this->WINDOW_WIDTH, this->WINDOW_HEIGHT));
	texts[0]->loadFont("fonts/open-sans/OpenSans-Bold.ttf");
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
	this->initSceneObjects();
}

MIA::~MIA()
{
	for (size_t i = 0; i < this->shaders.size(); i++)
		delete this->shaders[i];
	
	for (size_t i = 0; i < this->points.size(); i++)
		delete this->points[i];

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
		std::cout << "Mouse Pressed" << std::endl;
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
	static int		x_rot = 205;
	static int		y_rot = 239;
	static int		z_rot = 184;
	static float	scale = 1.0;
	static float pr1arr[2] = {50, 90};
	static float pr2arr[2] = {20, 90};
	static glm::vec3 m1(0), m2(0);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	{
        ImGui::Begin("Image Slicing");
		ImGui::PushItemWidth(100);



		ImGui::InputFloat2("U1, V1", pr1arr);
		ImGui::InputFloat2("U2, V2", pr2arr);
		if (ImGui::Button("Calculate"))
		{
			// Input
			glm::vec2 pr1(pr1arr[0], pr1arr[1]);
			glm::vec2 pr2(pr2arr[0], pr2arr[1]);
			// Find point:
			glm::vec3 cam1(0.0f, 0.0f, 0.0f);
			glm::vec3 cam2(-54.0f, 0.0f, 0.0f);
			float z = 200.0f;
			float f = z;
			float b = 54.0f;

		
			float r = b / (pr1.x - pr2.x);
			float s = r;

			m1 = glm::vec3(s * pr1.x, s * pr1.y, s * f);
			m2 = glm::vec3(b + r * pr2.x, r * pr2.y, r * f);
			std::cout << "Point1(" << m1.x << ", " << m1.y << ", " << m1.z << ")" << std::endl;
			std::cout << "Point2(" << m2.x << ", " << m2.y << ", " << m2.z << ")" << std::endl;
			if (m1 == m2)
			{
				points[2]->setPosition(glm::vec3(pr1arr[0]*10, pr1arr[1]*10, z*10));
				points[3]->setPosition(glm::vec3(pr2arr[0]*10, pr2arr[1]*10, z*10));
				points[4]->setPosition(m1 * 10.0f);
				for(int i = 2; i < points.size(); ++i)
					points[i]->activate();
				lines[3]->setPosition(cam1 * 10.0f, m1 * 10.0f);
				lines[4]->setPosition(cam2 * 10.0f, m2 * 10.0f);
				for(int i = 3; i < lines.size(); ++i)
					lines[i]->activate();

			}
			else
			{
				for(int i = 2; i < points.size(); ++i)
					points[i]->deactivate();
				for(int i = 3; i < lines.size(); ++i)
					lines[i]->deactivate();
			}
		}

		ImGui::Text("Camera1(%d, %d, %d)", 0, 0, 0);
		ImGui::Text("Camera2(%d, %d, %d)", -54, 0, 0);
		ImGui::Text("Camera1 Projection Point(%d, %d, %d)", (int)pr1arr[0], (int)pr1arr[1], 200);
		ImGui::Text("Camera2 Projection Point(%d, %d, %d)", (int)pr2arr[0], (int)pr2arr[1], 200);
		ImGui::Text("Point1(%.2f, %.2f, %.2f)", m1.x, m1.y, m1.z);
		ImGui::Text("Point2(%.2f, %.2f, %.2f)", m2.x, m2.y, m2.z);


		if (ImGui::Button("Rotate") ||
			ImGui::SliderInt("Rotate X", &x_rot, 0, 360) ||
			ImGui::SliderInt("Rotate Y", &y_rot, 0, 360) ||
			ImGui::SliderInt("Rotate Z", &z_rot, 0, 360) ||
			ImGui::SliderFloat("Scale",  &scale, 0.0f, 1.0f))
		{
			glm::mat4 rot = glm::mat4(1.0f);
			rot = glm::rotate(rot, (float)(x_rot * M_PI / 180), glm::vec3(1.0f, 0.0f, 0.0f));
			rot = glm::rotate(rot, (float)(y_rot * M_PI / 180), glm::vec3(0.0f, 1.0f, 0.0f));
			rot = glm::rotate(rot, (float)(z_rot * M_PI / 180), glm::vec3(0.0f, 0.0f, 1.0f));
			rot = glm::scale(rot, glm::vec3(scale));
			glm::mat4 ortho_proj = glm::ortho(-4000.0f, 4000.0f, -4000.0f, 4000.0f, -4000.0f, 4000.0f);
			rot = ortho_proj * rot;
			for (size_t i = 0; i < lines.size(); i++)
				lines[i]->setMVP(rot);
			for (size_t i = 0; i < points.size(); i++)
				points[i]->setMVP(rot);
			texts[0]->setMVP(rot);
		}

		ImGui::End();
    }
}

void MIA::render()
{
    ImGui::Render();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (size_t i = 0; i < lines.size(); i++)
		lines[i]->render(shaders[0]);
	for (size_t i = 0; i < points.size(); i++)
		points[i]->render(shaders[1]);
	texts[0]->renderText("X", 4000, 0, 0, 15, glm::vec3(1.0f, 0.0f, 0.0f));
	texts[0]->renderText("Y", 0, 4000, 0, 15, glm::vec3(0.0f, 1.0f, 0.0f));
	texts[0]->renderText("Z", 0, 0, 4000, 15, glm::vec3(0.0f, 0.0f, 1.0f));

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

//Static functions
void MIA::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
};



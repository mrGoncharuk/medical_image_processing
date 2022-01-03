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
	ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigWindowsMoveFromTitleBarOnly = true;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    // ImGui_ImplOpenGL3_Init(glsl_version);
    const char* glsl_version = "#version 330 core";
    ImGui_ImplOpenGL3_Init(glsl_version);
}


void MIA::initSceneObjects()
{
    std::string path = "/home/mhoncharuk/Education/MIA/data/PWI_680_slices_256x256";
	dct.readDicomSet(path);
	ss.init((unsigned short **)dct.getImageSet(), dct.params);
	this->textures.push_back(new Texture((char *)ss.getTransverseSlice(0, 0), dct.params["width"], dct.params["heigth"], dct.params));
	this->textures.push_back(new Texture((char *)ss.getSagittalSlice(0, 0), dct.params["width"], dct.params["length"]*dct.params["thickness"], dct.params));
	this->textures.push_back(new Texture((char *)ss.getFrontalSlice(0, 0), dct.params["heigth"], dct.params["length"]*dct.params["thickness"], dct.params));
    tc.init(dct.params["TE"], dct.params["repetitionTime"]);
}

//Constructors / Destructors
MIA::MIA(
	const char* title,
	const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
	bool resizable
)
	:
	WINDOW_WIDTH(WINDOW_WIDTH),
	WINDOW_HEIGHT(WINDOW_HEIGHT)
{
	//Init variables
	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->WINDOW_HEIGHT;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;

	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLAD();
	this->initOpenGLOptions();
    this->initImGui();

	this->initSceneObjects();
}

MIA::~MIA()
{
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	
	for (size_t i = 0; i < this->textures.size(); i++)
		delete this->textures[i];


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

void MIA::updateMouseInput()
{

}

void MIA::updateKeyboardInput()
{
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
	this->updateInput();
	
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	static int transverse_slice = 0;
    static int transverse_time = 0;
    static ImVec2 transverse_point(0, 0);

    static int sagittal_slice = 0;
    static int sagittal_time = 0;
    static ImVec2 sagittal_point(0, 0);

    static int frontal_slice = 0;
    static int frontal_time = 0;
    static ImVec2 frontal_point(0, 0);

    static std::vector<double> pointInTime;
    
	{
        ImGui::Begin("Transverse");

        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImVec2(dct.params["width"], dct.params["heigth"]);
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Image((void*)(intptr_t)textures[0]->getID(), ImVec2(256, 256));

        // This will catch our interactions
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x, io.MousePos.y - canvas_p0.y);
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            sagittal_point = ImVec2(0,0);
            frontal_point = ImVec2(0,0);
            transverse_point = mouse_pos_in_canvas;
            pointInTime = ss.getPointInTime(Plane::TRANSVERSE, transverse_slice, transverse_point.x, transverse_point.y);
            tc.setData(pointInTime);
        }

        ImGui::PushItemWidth(100);
        if (ImGui::SliderInt("Transverse Slice", &transverse_slice, 0, dct.params["length"] - 1) ||
            ImGui::SliderInt("Transverse Time", &transverse_time, 0, dct.params["time"] - 1))
            textures[0]->reloadFromData((char *)ss.getTransverseSlice(transverse_slice, transverse_time), dct.params["width"], dct.params["heigth"], dct.params);
        ImGui::Text("Transverse Point = (%0.f, %0.f)", transverse_point.x, transverse_point.y);
        ImGui::End();
    }

	{
        ImGui::Begin("Sagittal");

        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImVec2(dct.params["width"], dct.params["heigth"]);
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Image((void*)(intptr_t)textures[1]->getID(), ImVec2(256, dct.params["length"]*dct.params["thickness"]));
        // This will catch our interactions
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x, io.MousePos.y - canvas_p0.y);
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            frontal_point = ImVec2(0,0);
            transverse_point = ImVec2(0,0);
            sagittal_point = mouse_pos_in_canvas;
            pointInTime = ss.getPointInTime(Plane::SAGITTAL, sagittal_slice, sagittal_point.x, sagittal_point.y);
            tc.setData(pointInTime);
        }

        ImGui::PushItemWidth(100);
        if (ImGui::SliderInt("Sagittal Slice", &sagittal_slice, 0, dct.params["heigth"] - 1) ||
            ImGui::SliderInt("Sagittal Time", &sagittal_time, 0, dct.params["time"] - 1))
            textures[1]->reloadFromData((char *)ss.getSagittalSlice(sagittal_slice, sagittal_time), dct.params["heigth"], dct.params["length"]*dct.params["thickness"], dct.params);
        ImGui::Text("Sagittal Point = (%0.f, %0.f)", sagittal_point.x, sagittal_point.y);
        ImGui::End();
    }

	{
        ImGui::Begin("Frontal");
        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImVec2(dct.params["width"], dct.params["heigth"]);
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Image((void*)(intptr_t)textures[2]->getID(), ImVec2(256, dct.params["length"]*dct.params["thickness"]));
        // This will catch our interactions
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x, io.MousePos.y - canvas_p0.y);
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            sagittal_point = ImVec2(0,0);
            transverse_point = ImVec2(0,0);
            frontal_point = mouse_pos_in_canvas;
            pointInTime = ss.getPointInTime(Plane::FRONTAL, frontal_slice, frontal_point.x, frontal_point.y);
            tc.setData(pointInTime);
        }

        ImGui::PushItemWidth(100);
        if (ImGui::SliderInt("Frontal Slice", &frontal_slice, 0, dct.params["width"] - 1) ||
            ImGui::SliderInt("Frontal Time", &frontal_time, 0, dct.params["time"] - 1))
            textures[2]->reloadFromData((char *)ss.getFrontalSlice(frontal_slice, frontal_time), dct.params["width"], dct.params["length"]*dct.params["thickness"], dct.params);
        ImGui::Text("Frontal Point = (%0.f, %0.f)", frontal_point.x, frontal_point.y);
        
        ImGui::End();
    }


    {
        ImGui::Begin("Time-Conecntration");
        double xs[40];

        for (int i = 0; i < 40; ++i)
            xs[i] = i * 1.5;
        if (pointInTime.size())
        {
            ImPlot::BeginPlot("Line Plot");
            ImPlot::SetupAxes("t(sec)","signal");
            ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
            ImPlot::PlotLine("TimeConcentration", xs, tc.getCurve().data(), dct.params["time"]);
            ImPlot::EndPlot();
        }
        ImGui::Text("TTA: %f", tc.getTTA());
        ImGui::Text("baseline: %f", tc.getbaseline());
        ImGui::Text("PE: %f", tc.getPE());
        ImGui::Text("TTP: %f", tc.getTTP());
        ImGui::Text("rTTP: %f", tc.getrTTP());
        ImGui::Text("FWHM: %f", tc.getFWHM());
        ImGui::Text("WiR: %f", tc.getWiR());
        ImGui::Text("WoR: %f", tc.getWoR());
        ImGui::End();
    }

	// ROI Data
	ImGui::ShowDemoWindow();
}

void MIA::render()
{
    ImGui::Render();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

//Static functions
void MIA::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
};



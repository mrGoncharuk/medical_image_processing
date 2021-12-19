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
    std::string path = "/home/mhoncharuk/Education/MIA/data/DICOM_set_16bits";
	dct.readDicomSet(path);
	ss.init((short **)dct.getImageSet(), dct.params["width"], dct.params["heigth"], dct.params["length"]);
	this->textures.push_back(new Texture((char *)ss.getTransverseSlice(0), dct.params["width"], dct.params["heigth"], dct.params));
	this->textures.push_back(new Texture((char *)ss.getTransverseSlice(0), dct.params["width"], dct.params["heigth"], dct.params));

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
	// if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	// {
	// 	std::cout << "Mouse Pressed" << std::endl;
	// }
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

cv::Mat		cropImage(short *img, ImVector<ImVec2> points)
{
	float x = points[0].x < points[1].x ? points[0].x : points[1].x;
	float y = points[0].y < points[1].y ? points[0].y : points[1].y;
	float w = abs(points[0].x - points[1].x);
	float h = abs(points[0].y - points[1].y);


	std::cout << "x: " << x << " y: " << y << " w: " << w << " h: " << h << std::endl;
	int histSize = 0;
	cv::Mat img_hist;
	cv::Mat image = cv::Mat(256, 256, CV_16SC1, (void *)img);
	cv::Mat croppedImage = image(cv::Rect(x, y, w, h));
	cv::Mat croppedImageU16;
	croppedImage.convertTo(croppedImageU16, CV_16U);
	
	return croppedImageU16;
}

cv::Mat		calculateFrequencies(cv::Mat image)
{
	cv::Mat hist;
	
	double minVal;
	double maxVal;
	cv::Point minLoc; 
	cv::Point maxLoc;
	minMaxLoc( image, &minVal, &maxVal, &minLoc, &maxLoc );
	std::cout << "Max val: " << maxVal << std::endl;
	int histSize = maxVal;
	float range[] = { 0,(float)maxVal }; //the upper boundary is exclusive
	const float* histRange[] = { range };
	cv::calcHist( &image, 1, 0, cv::Mat(), hist, 1, &histSize, histRange, true, false );

	cv::Mat hist_int;
	hist.convertTo(hist_int, CV_32S);
	return hist_int;
}



void MIA::update()
{
	this->updateInput();
	
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

	// ROI Data
	static ImVector<ImVec2> points;

	// Statistics Data
	static bool show_statistics = false;

	// Histogram Data
	static int		img1_slice = 0;
	static cv::Mat 	img1_hist;
	static cv::Mat 	img1_roi;

	static int		img2_slice = 0;
	static cv::Mat	img2_hist;
	static cv::Mat	img2_roi;
	
	static bool show_plot = false;
	static bool show_plot2 = false;
	// static bool adding_line2 = false;



	// ROI Selection
	{
        ImGui::Begin("ROI1 Analysis");

		static ImVec2 scrolling(0.0f, 0.0f);
		static bool adding_line = false;
		
		static bool opt_enable_context_menu = true;

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImVec2(256, 256);   // Resize canvas to what's available
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::Image((void*)(intptr_t)textures[0]->getID(), ImVec2(256, 256));
		
		// This will catch our interactions
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held
		const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

		// // Add first and second point
		if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (points.Size == 2)
				points.clear();
			points.push_back(mouse_pos_in_canvas);
			points.push_back(mouse_pos_in_canvas);
			adding_line = true;
		}
		if (adding_line)
		{
			points.back() = mouse_pos_in_canvas;
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{

				adding_line = false;
			}
		}
		// Draw grid + all lines in the canvas
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		
		for (int n = 0; n < points.Size; n += 2)
			draw_list->AddRect(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255));
		draw_list->PopClipRect();

		ImGui::Checkbox("Show Plot", &show_plot); ImGui::SameLine();
		ImGui::Checkbox("Show Statistics", &show_statistics);
		ImGui::PushItemWidth(100);
        if (ImGui::SliderInt("Slice1", &img1_slice, 0, 19))
            textures[0]->reloadFromData((char *)ss.getTransverseSlice(img1_slice), dct.params["width"], dct.params["heigth"], dct.params);

        ImGui::End();
    }


	{
        ImGui::Begin("ROI2 Analysis");

		static ImVec2 scrolling(0.0f, 0.0f);
		static bool adding_line = false;
		static bool opt_enable_context_menu = true;

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImVec2(256, 256);   // Resize canvas to what's available
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::Image((void*)(intptr_t)textures[1]->getID(), ImVec2(256, 256));
		
		// This will catch our interactions
		const bool is_hovered = ImGui::IsItemHovered(); // Hovered
		const bool is_active = ImGui::IsItemActive();   // Held
		const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

		// // Add first and second point
		if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			if (points.Size == 2)
				points.clear();
			points.push_back(mouse_pos_in_canvas);
			points.push_back(mouse_pos_in_canvas);
			adding_line = true;
		}
		if (adding_line)
		{
			points.back() = mouse_pos_in_canvas;
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
				adding_line = false;
		}
		// Draw grid + all lines in the canvas
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		
		for (int n = 0; n < points.Size; n += 2)
			draw_list->AddRect(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255));
		draw_list->PopClipRect();

		ImGui::Checkbox("Show Plot", &show_plot2); ImGui::SameLine();
		// ImGui::Checkbox("Show Statistics", &show_statistics);
		ImGui::PushItemWidth(100);
        if (ImGui::SliderInt("Slice2", &img2_slice, 0, 19))
            textures[1]->reloadFromData((char *)ss.getTransverseSlice(img2_slice), dct.params["width"], dct.params["heigth"], dct.params);

        ImGui::End();
    }

	if (show_plot)
	{
        ImGui::Begin("Plotter1");
		if (ImGui::Button("Calculate ROI1 Histogram") && points.Size == 2 && abs(int(points[0].x - points[1].x)) && abs(int(points[0].x - points[1].x)))
		{
			img1_roi = cropImage(ss.getTransverseSliceOriginal(img1_slice), points);
			img1_hist = calculateFrequencies(img1_roi);
			cts.setROI(img1_roi);
			cts.setHistogram(img1_hist);
			img2_roi = cropImage(ss.getTransverseSliceOriginal(img2_slice), points);
			img2_hist = calculateFrequencies(img2_roi);
			cts.setData2D(img2_roi, img2_hist);
			cts.recalculate();
		}
		
		ImPlot::BeginPlot("ROI1 histogram", ImVec2(-1, 0), ImPlotFlags_NoLegend);
		ImPlot::SetupAxes("Intensity", "Frequency");
		ImPlot::SetupAxesLimits(0,1500,0,200);
		ImPlot::PlotBars("Intensity", img1_hist.ptr<int>(0), img1_hist.total(), 5);

		ImPlot::EndPlot();
        ImGui::End();
    }

	if (show_plot2)
	{
        ImGui::Begin("Plotter2");
		if (ImGui::Button("Calculate ROI2 Histogram") && points.Size == 2)
		{
			img1_roi = cropImage(ss.getTransverseSliceOriginal(img1_slice), points);
			img1_hist = calculateFrequencies(img1_roi);
			cts.setROI(img1_roi);
			cts.setHistogram(img1_hist);
			img2_roi = cropImage(ss.getTransverseSliceOriginal(img2_slice), points);
			img2_hist = calculateFrequencies(img2_roi);
			cts.setData2D(img2_roi, img2_hist);
			cts.recalculate();
		}
		
		ImPlot::BeginPlot("ROI2 histogram", ImVec2(-1, 0), ImPlotFlags_NoLegend);
		ImPlot::SetupAxes("Intensity", "Frequency");
		ImPlot::SetupAxesLimits(0,1500,0,200);
		ImPlot::PlotBars("Intensity", img2_hist.ptr<int>(0), img2_hist.total(), 5);

		ImPlot::EndPlot();
        ImGui::End();
    }
	// Statistics Calculation
	if (show_statistics)
	{
		float empty = 0;
		ImGui::Begin("Statistics");
		ImGui::Text("Mathematical Expectation: %f", cts.getMathExpectation());
		ImGui::Text("2D Entropy: %f",cts.getEntropy2D());
		ImGui::Text("2D Energy: %f",cts.getEnergy2D());
		ImGui::Text("Asymmetry Coefficient: %f", cts.getAsymmetryCoef());
		ImGui::Text("Mean Absolute Difference: %f", cts.getMeanAbsDiff());

		ImGui::End();
	}
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



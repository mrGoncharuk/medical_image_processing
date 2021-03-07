#include "GUI.hpp"

using namespace std;

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


GUI::GUI(/* args */): clearColor(0.45f, 0.55f, 0.60f, 1.00f)
                    , loadedDataSet(imebra::CodecFactory::load("data/copy_of_DICOM_Image_for_Lab_2.dcm"))
{

}


GUI::~GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
}


bool GUI::initGL()
{
        // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::cerr << "ERROR: Failed initialization of GLFW" << std::endl;
        return false;
    }
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
    this->window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Medical Image Processing", NULL, NULL);
    if (this->window == NULL)
    {
        std::cerr << "ERROR: Failed creation of window" << std::endl;
        return false;
    }
    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(1); // Enable vsync

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
        return false;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    image_renderer.createShaderProgram("shaders/vert.shader", "shaders/frag.shader");
    image_renderer.loadImage("data/copy_of_DICOM_Image_for_Lab_2.dcm");


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
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}



void    GUI::events(std::atomic<bool> &isRunning)
{
    glfwPollEvents();
    if (glfwWindowShouldClose(this->window))
        isRunning = false;

}

void	GUI::update()
{
    // memset(&(this->flags), 0, sizeof(flags));
    static bool show_demo_window = true;
    static int maskType = 4;
    static int LUTType = 0;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    // if (show_demo_window)
    //     ImGui::ShowDemoWindow(&show_demo_window);

    // {
    //     ImGui::Begin("Image")
    //     ImGui::Image((void*)(intptr_t)this->my_image_texture, ImVec2(width, height));
    //     ImGui::End();
    // }

    {
        ImGui::Begin("Histogram transformation");

        static float red[256] = {0};
        static float blue[256] = {0};
        static float green[256] = {0};
        static float gray[256] = {0};

        static float max_red = 0.0f;
        static float max_green = 0.0f;
        static float max_blue = 0.0f;
        static float max_gray = 0.0f;
        
        if (ImGui::Button("Build histogram"))
        {
            int length = this->image_renderer.getWidth() * this->image_renderer.getHeight() * this->image_renderer.getChannels();
            const unsigned char *image_data = this->image_renderer.getImageData();
            const float r_gray = 0.3;
            const float g_gray = 0.6;
            const float b_gray = 0.1;

            memset(red, 0, sizeof(float) * 256);
            memset(green, 0, sizeof(float) * 256);
            memset(blue, 0, sizeof(float) * 256);
            memset(gray, 0, sizeof(float) * 256);

            for(int i=0; i < length; i += this->image_renderer.getChannels())
            {
                red[image_data[i]] += 1.0f;
                green[image_data[i+1]] += 1.0f;
                blue[image_data[i+2]] += 1.0f;
                int grayscale = roundf((image_data[i] * r_gray) + (image_data[i+1] * g_gray) +
                    (image_data[i+2] * b_gray));
                gray[grayscale] += + 1.0f;
            }
            // red[0] = 0;
            // green[0] = 0;
            // blue[0] = 0;
            // gray[0] = 0;

            // compute max
            for(int i=0; i < 256; i++)
            {
                max_red = MAX(red[i], max_red);
                max_green = MAX(green[i], max_green);
                max_blue = MAX(blue[i], max_blue);
                max_gray = MAX(gray[i], max_gray);
            }
        }

        ImGui::PlotHistogram("", red, 256, 0, "RED Histogram", 0.0f, max_red, ImVec2(400, 250));
        ImGui::SameLine();
        ImGui::PlotHistogram("", green, 256, 0,"GREEN Histogram", 0.0f, max_green,
        ImVec2(400,250));
        ImGui::PlotHistogram("", blue, 256, 0, "BLUE Histogram", 0.0f, max_blue, ImVec2(400,250));
        ImGui::SameLine();
        ImGui::PlotHistogram("", gray, 256, 0, "GRAY Histogram", 0.0f, max_gray, ImVec2(400,250));

        ImGui::End();
    }

    {
        ImGui::Begin("Image Operations");

        if (ImGui::Button("Restore Image"))
        {
            this->image_renderer.restoreImageData();
            this->image_renderer.redrawImage();
            maskType = 4;
            LUTType = 0;
        }
        ImGui::RadioButton("Vertical Left", &maskType, 0);
		ImGui::RadioButton("Vertical Right", &maskType, 1);
		ImGui::RadioButton("Horizontal Up", &maskType, 2);
        ImGui::RadioButton("Horizontal Down", &maskType, 3);
        ImGui::RadioButton("None", &maskType, 4);
        if (ImGui::Button("Apply Mask"))
        {
            if (maskType == 4)
            {
                this->image_renderer.restoreImageData();
                this->image_renderer.redrawImage();
            }
            else
            {
                unsigned char *mask = generateMask(this->image_renderer.getWidth(), this->image_renderer.getHeight(), maskType);
                applyMask(  this->image_renderer.getImageData(),
                            mask,
                            this->image_renderer.getWidth(),
                            this->image_renderer.getHeight(),
                            this->image_renderer.getChannels());
                this->image_renderer.redrawImage();
                delete [] mask;
            }
        }

        ImGui::RadioButton("Standart Image", &LUTType, 0);
		ImGui::RadioButton("Default LUT(0..255)", &LUTType, 1);
		ImGui::RadioButton("Inversed LUT(255..0)", &LUTType, 2);

        if (ImGui::Button("Apply LUT"))
        {
            if (!LUTType)
                this->image_renderer.restoreImageData();
            else
                applyLUTRedChannel( this->image_renderer.getImageData(),
                                    generateLUT(LUTType == 1 ? 1 : 0),
                                    this->image_renderer.getWidth(),
                                    this->image_renderer.getHeight(),
                                    this->image_renderer.getChannels());
            this->image_renderer.redrawImage();
        }
        ImGui::End();
    }
}



void	GUI::render()
{
    static char buf[3];
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    // glBindFramebuffer(GL_FRAMEBUFFER, this->my_image_texture);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    // glMatrixMode(GL_PROJECTION)
    glClear(GL_COLOR_BUFFER_BIT);
    image_renderer.renderImage();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}


GLFWwindow *GUI::getWindow() { return (this->window); }


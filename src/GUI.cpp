#include "GUI.hpp"

using namespace std;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


GUI::GUI(/* args */): clearColor(0.45f, 0.55f, 0.60f, 1.00f)
                    , loadedDataSet(imebra::CodecFactory::load("data/DICOM_Image_for_Lab_2.dcm"))
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
    image_renderer.loadImage("data/Lab2-6DICOM_single_16bits/DICOM_Image_16b.dcm");


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
    static bool origHistIsReady = false;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("Histogram transformation");


        static float gray_orig[1024] = {0};
        static float max_gray_orig = 0.0f;
        static float gray_current[65536] = {0};
        static float max_gray_current = 0.0f;
        static int threshold = 0;
        static unsigned short *segmented_image = new unsigned short[this->image_renderer.getWidth() * this->image_renderer.getHeight()];
        static int sigmas[2] = {0};
        if (!origHistIsReady)
        {
            int length = this->image_renderer.getWidth() * this->image_renderer.getHeight() * this->image_renderer.getChannels();
            const unsigned short *image_data = this->image_renderer.getImageData();

            // memset(gray_orig, 0, sizeof(float) * 256);
            countHistogram(this->image_renderer.getImageData(),
                            this->image_renderer.getWidth() * this->image_renderer.getHeight(),
                            this->image_renderer.getChannels(),
                            gray_orig, &max_gray_orig);
            max_gray_current = max_gray_orig;
            for (int i = 0; i < sizeof(gray_orig) / sizeof(float); i++)
                gray_current[i] = gray_orig[i];
            origHistIsReady = true;
        }

        if (ImGui::Button("Restore Image"))
        {
            this->image_renderer.restoreImageData();
            this->image_renderer.redrawImage();
        }

        if (ImGui::Button("Equalization"))
        {
            this->image_renderer.restoreImageData();
            equalizeHistogram(this->image_renderer.getImageData(),
                                    this->image_renderer.getWidth(),
                                    this->image_renderer.getHeight(),
                                    this->image_renderer.getChannels(), USHRT_MAX);
            this->image_renderer.redrawImage();
        }

        if (ImGui::Button("Apply Segmentation Mask"))
        {
            applyMask(this->image_renderer.getImageData(),
                        segmented_image,
                        this->image_renderer.getWidth(),
                        this->image_renderer.getHeight(),
                        this->image_renderer.getChannels());
            this->image_renderer.redrawImage();
        }

        if (ImGui::Button("Color Segmented Region"))
        {
            applyColorMask(this->image_renderer.getImageData(),
                        segmented_image,
                        this->image_renderer.getWidth(),
                        this->image_renderer.getHeight(),
                        this->image_renderer.getChannels());
            this->image_renderer.redrawImage();
    
        }

        if (ImGui::SliderInt2("Sigma", sigmas, 0, 10))
        {
            int image_length = this->image_renderer.getWidth()* this->image_renderer.getHeight();
            this->image_renderer.restoreImageData();

            DoG(this->image_renderer.getImageData(),
                                    this->image_renderer.getWidth(),
                                    this->image_renderer.getHeight(),
                                    this->image_renderer.getChannels(), sigmas[0], sigmas[1]);
            initSingleChannelImage(this->image_renderer.getImageData(), segmented_image, image_length, this->image_renderer.getChannels());
            this->image_renderer.redrawImage();
        }

        ImGui::PlotHistogram("", gray_orig, 1024, 0, "Original Image Histogram", 0.0f, max_gray_orig, ImVec2(400,250));
        ImGui::Text("0");ImGui::SameLine(380);ImGui::Text("1024");
        ImGui::End();
    }
}

void	GUI::render()
{
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


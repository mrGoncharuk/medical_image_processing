#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "Graphics.hpp"

#include "DicomCT.hpp"
#include "SetSlicer.hpp"
#include "CTStatist.hpp"
#include "TimeConcentration.hpp"

#include "Texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class MIA
{
private:
//Variables
	//Window
	GLFWwindow* window;
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	int framebufferWidth;
	int framebufferHeight;

	//Mouse Input
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;

	//Textures
	std::vector<Texture*> textures;

	// CT data
	DicomCT dct;
	SetSlicer<unsigned short> ss;
	CTStatist cts;
	TimeConcentration tc;
	//Private functions
	void initGLFW();
	void initWindow(
		const char* title,
		bool resizable
	);
	void initGLAD(); //AFTER CONTEXT CREATION!!!
	void initOpenGLOptions();

	// void initMatrices();
    void initSceneObjects();
    void initImGui();

public:
//Constructors / Destructors
	MIA(
		const char* title,
		const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
		bool resizable
	);
	virtual ~MIA();

//Accessors
	int getWindowShouldClose();

//Modifiers
	void setWindowShouldClose();

//Functions
	void updateMouseInput();
	void updateKeyboardInput();
	void updateInput();
	void update();
	void render();

//Static functions
	static void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH);
};

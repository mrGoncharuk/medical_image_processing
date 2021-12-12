#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Graphics.hpp"

#include "Shader.hpp"
#include "Line.hpp"
#include "Point.hpp"
#include "Text.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


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

	//Delta time
	float dt;
	float curTime;
	float lastTime;

	//Mouse Input
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;

	int rulerState;	// 0 - off, 1 - the first point, 2 - the second point
	//Shaders
	std::vector<Shader*> shaders;

	//Lines
	std::vector<Line*> lines;

	// Points
	std::vector<Point*> points;

	std::vector<Text *> texts;

	//Private functions
	void initGLFW();
	void initWindow(
		const char* title,
		bool resizable
	);
	void initGLAD(); //AFTER CONTEXT CREATION!!!
	void initOpenGLOptions();

	// void initMatrices();
	void initShaders();
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
	void updateDt();
	void updateMouseInput();
	void updateKeyboardInput();
	void updateInput();
	void update();
	void render();

//Static functions
	static void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH);
};

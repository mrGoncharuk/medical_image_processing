#include "GUI.hpp"


int		main(int argc, char **argv)
{
	std::atomic<bool> isRunning;
	GUI gui;

	isRunning = true;
	if (gui.initGL() == false)
		isRunning = false;
	while (isRunning)
	{
		gui.events(isRunning);
		gui.update();
		gui.render();
	}

    return 0;
}
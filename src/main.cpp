#include "MIA.hpp"

int main()
{
	MIA game("Medical Image Analysis",
		800, 800,
		false);

	//MAIN LOOP
	while (!game.getWindowShouldClose())
	{
		//UPDATE INPUT ---
		game.update();
		game.render();	
	}

	return 0;
}

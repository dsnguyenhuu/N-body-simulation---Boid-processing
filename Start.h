#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include "Bird.h"
#include "Flock.h"


class Start {
public:
	static int SCREEN_WIDTH;
	static int SCREEN_HEIGHT;
		
	bool initialize(char* windowTitle);

	void update();
	void render(std::vector<Flock> flock);

	static double limitFPS;

	double last = glfwGetTime();
	double timer = last;

	double timeDif = 0;
	double now = 0;
	
private:
	static GLFWwindow* window;

	std::vector<std::thread*> threads;
	std::vector<Flock> flock;

};




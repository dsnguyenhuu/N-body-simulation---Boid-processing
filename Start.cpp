//Dong-Son Nguyen-Huu - 40014054
//COMP 426 
//October 8, 2017
//Assignment 1

#include "Start.h"
#include "tbb/task_group.h"

//initialize the parameters for the window
int Start::SCREEN_WIDTH = 1500;
int Start::SCREEN_HEIGHT = 1000;
GLFWwindow* Start::window = NULL;

//static number to be used for FPS control
double Start::limitFPS = 1 / 15.0;

bool Start::initialize(char *windowTitle) {

	if (!glfwInit()){
		std::cout << "Error starting GLFW" << std::endl;
		return false;
	}

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle, NULL, NULL);
	if (window == NULL)	{
		std::cout << "Error creating window" << std::endl;
		return false;
	}

	//GLFW Setup
	glfwMakeContextCurrent(window);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glfwSwapInterval(1);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int xPos = (mode->width - SCREEN_WIDTH) / 2;
	int yPos = (mode->height - SCREEN_HEIGHT) / 2;
	glfwSetWindowPos(window, xPos, yPos);
	
	return true;
}

void Start::update() {

	// - Measure time
	now = glfwGetTime();
	timeDif += (now - last) / limitFPS;
	last = now;
	
	glfwPollEvents();
}

//void Start::render(std::vector<Flock> flock) {
//	//setting OpenGL window color
//	glClearColor(1, 1, 1, 1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	// - Only update at 30 frames 
//	//each bird will update itself onto another thread
//	while (timeDif >= 1.0) {
//		for (int i = 0; i<flock.size(); i++) {
//			//flock[i].updateBird();
//
//			std::thread* t = new std::thread(&Flock::updateBird, flock[i]);
//			t->join(); //threads rejoin after completion to avoid high memory use
//
//	}
//		timeDif--;
//	}
//	//iterate through every bird and draw its new position
//	for (int i = 0; i<flock.size(); i++) {
//		for (int j = 0; j <flock[i].getFlock().size(); j++) {
//			flock[i].getFlock()[j]->drawBird();
//		}
//	}
//	
//	glfwSwapBuffers(window);
//
//}

void Start::render(std::vector<Flock> flock) {
	//setting OpenGL window color
	glClearColor(0.1, 0.2, 0.3, 0.4);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// - Only update at 30 frames 
	//each bird will update itself onto another thread
	while (timeDif >= 1.0) {

		tbb::task_group g;
	/*	g.run([&] {flock[0].updateBird(); });
		g.run([&] {flock[1].updateBird(); });
		g.run([&] {flock[2].updateBird(); });
		g.run([&] {flock[3].updateBird(); });
		g.run([&] {flock[4].updateBird(); });
		g.run([&] {flock[5].updateBird(); });
		g.run([&] {flock[6].updateBird(); });
		g.wait();*/

		for (int i = 0; i < flock.size(); i++) {
		g.run([&] {flock[i].updateBird(); });
		g.wait();
		}
		timeDif--;
	}




	//iterate through every bird and draw its new position
	for (int i = 0; i<flock.size(); i++) {
		for (int j = 0; j <flock[i].getFlock().size(); j++) {
			flock[i].getFlock()[j]->drawBird();
		}
	}

	glfwSwapBuffers(window);

}
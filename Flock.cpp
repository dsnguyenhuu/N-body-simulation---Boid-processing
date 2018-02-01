//Dong-Son Nguyen-Huu - 40014054
//COMP 426 
//October 8, 2017
//Assignment 1

#include <random>
#include <iostream>
#include "Flock.h"

#define MAX_SOURCE_SIZE (0x100000)
#define NB_OF_BOIDS 25

size_t Flock::global_item_size;
size_t Flock::local_item_size;

cl_platform_id Flock::platform_id;
cl_device_id Flock::device_id;
cl_uint Flock::ret_num_devices;
cl_uint Flock::ret_num_platforms;

cl_int Flock::ret;
cl_context Flock::context;
cl_program Flock::program;

cl_kernel Flock::velocity_kernel;
cl_kernel Flock::position_kernel;
cl_command_queue Flock::command_queue;

cl_mem Flock::dev_old_velocity;
cl_mem Flock::dev_cohesion;
cl_mem Flock::dev_separation;
cl_mem Flock::dev_alignment;
cl_mem Flock::dev_new_velocity;
cl_mem Flock::dev_old_position;
cl_mem Flock::dev_new_position;

//Initialize the static float 'color'
float Flock::color = 0.15;

//Initializing the random device to be used
std::random_device rd2;
std::mt19937 e23(rd2());

Flock::Flock(int numOfBirds) {
	//Iterate through all the birds of that flock
	for (int i = 0; i <numOfBirds; i++) {
		//generate the bird with the same color
		auto * bird = new Bird(0.1 + color, color, 0.3 + color);
		//push the birds into the flock (which is a vector of birds)
		flock.push_back(bird);
	}
	//increase the color value to change the appearance between flocks
	color += 0.08;

	FILE *fp;
	char fileName[] = "./gpu_kernel.cl";
	char *source_str;
	size_t source_size;

	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}

	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	global_item_size = NB_OF_BOIDS * 2; // Process the entire lists
	local_item_size = 1; // Process in groups of 1

						 // Get platform and device information
	platform_id = NULL;
	device_id = NULL;

	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

	// Create an OpenCL context
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	// Create a program from the kernel source
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	// Create the OpenCL kernel
	velocity_kernel = clCreateKernel(program, "computeNewVelocity", &ret);
	position_kernel = clCreateKernel(program, "computeNewPosition", &ret);

	// Create a command queue
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	// Create memory buffers on the device for each vector 
	dev_old_velocity = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);

	dev_cohesion = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);
	dev_separation = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);
	dev_alignment = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);

	dev_new_velocity = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);

	dev_old_position = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);

	dev_new_position = clCreateBuffer(context, CL_MEM_READ_ONLY, NB_OF_BOIDS * 2 * sizeof(float), NULL, NULL);

}

//Getter function to return the current flock
const std::vector<Bird *> &Flock::getFlock() const {
	return flock;
}


//Function that will adjust the positioning of the boids according to the 3 rules
void Flock::updateBird() {
	
	float old_velocity[NB_OF_BOIDS * 2];

	float cohesion[NB_OF_BOIDS * 2];
	float separation[NB_OF_BOIDS * 2];
	float alignment[NB_OF_BOIDS * 2];

	float new_velocity[NB_OF_BOIDS * 2];

	float old_position[NB_OF_BOIDS * 2];
	float new_position[NB_OF_BOIDS * 2];

	
	

	//Initialize the 3 vectors that correspond to the rules
	Eigen::Vector2f v1, v2, v3;
			
	//iterate through each boid in a given flock
	for (int i = 0; i < flock.size(); i++) {
		old_position[i] = flock[i]->getPosition()(0);
		old_position[i + NB_OF_BOIDS] = flock[i]->getPosition()(1);
		
		old_velocity[i] = flock[i]->getVelocity()(0);
		old_velocity[i + NB_OF_BOIDS] = flock[i]->getVelocity()(1);


		//Rule 1: Boids try to fly towards the centre of a mass of neighboring boids		
		v1 = rule1(flock[i]);
		//Rule 2: Boids try to keep a small distance away from other objects (including other boids).
		v2 = rule2(flock[i]);
		//Rule 3: Boids try to match velocity with near boids.
		v3 = rule3(flock[i]);

		cohesion[i] = v1(0);
		cohesion[NB_OF_BOIDS + i] = v1(1);

		separation[i] = v2(0);
		separation[NB_OF_BOIDS + i] = v2(1);

		alignment[i] = v3(0);
		alignment[NB_OF_BOIDS + i] = v3(1);
			
	}
	// New Velocity

	// Enqueue
	ret = clEnqueueWriteBuffer(command_queue, dev_old_velocity, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), old_velocity, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, dev_cohesion, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), cohesion, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, dev_separation, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), separation, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, dev_alignment, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), alignment, 0, NULL, NULL);

	// Set the arguments of the kernel
	ret = clSetKernelArg(velocity_kernel, 0, sizeof(cl_mem), (void *)&dev_old_velocity);
	ret = clSetKernelArg(velocity_kernel, 1, sizeof(cl_mem), (void *)&dev_cohesion);
	ret = clSetKernelArg(velocity_kernel, 2, sizeof(cl_mem), (void *)&dev_separation);
	ret = clSetKernelArg(velocity_kernel, 3, sizeof(cl_mem), (void *)&dev_alignment);

	ret = clSetKernelArg(velocity_kernel, 4, sizeof(cl_mem), (void *)&dev_new_velocity);

	// Execute the OpenCL kernel on the list
	ret = clEnqueueNDRangeKernel(command_queue, velocity_kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	// Read the memory buffer C on the device to the local variable C
	ret = clEnqueueReadBuffer(command_queue, dev_new_velocity, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), new_velocity, 0, NULL, NULL);

	// Finish Queue
	ret = clFinish(command_queue);

	// New position

	// Enqueue
	ret = clEnqueueWriteBuffer(command_queue, dev_old_position, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), old_position, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, dev_new_velocity, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), new_velocity, 0, NULL, NULL);

	// Set the arguments of the kernel
	ret = clSetKernelArg(position_kernel, 0, sizeof(cl_mem), (void *)&dev_old_position);
	ret = clSetKernelArg(position_kernel, 1, sizeof(cl_mem), (void *)&dev_new_velocity);
	ret = clSetKernelArg(position_kernel, 2, sizeof(cl_mem), (void *)&dev_new_position);

	// Execute the OpenCL kernel on the list
	ret = clEnqueueNDRangeKernel(command_queue, position_kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	// Read the memory buffer C on the device to the local variable C
	ret = clEnqueueReadBuffer(command_queue, dev_new_position, CL_TRUE, 0, NB_OF_BOIDS * 2 * sizeof(float), new_position, 0, NULL, NULL);

	// Clean up
	ret = clFinish(command_queue);

	for (int i = 0; i < flock.size(); i++) {
		Eigen::Vector2f tempVel, tempPos;
		tempVel(0) = new_velocity[i];
		tempVel(1) = new_velocity[i + NB_OF_BOIDS];
		tempPos(0) = new_position[i];
		tempPos(1) = new_position[i + NB_OF_BOIDS];

		flock[i]->setVelocity(tempVel);
		flock[i]->setPosition(tempPos);
		boundPosition(flock[i]);
		limitSpeed(flock[i]);
	}


}
//Rule 1: Boids try to fly towards the centre of mass of neighboring boids
Eigen::Vector2f Flock::rule1(Bird *birdj) {
	//Creating a vector pcJ, which will eventually calculate the centre of all other boids of its flock
	Eigen::Vector2f pcJ; 
	pcJ(0) = 0;
	pcJ(1) = 0;

	//iterate through the whole flock
	for (int i = 0; i <flock.size(); i++) {
		if (flock[i] != birdj) {				//only continues if it is another boid and not itself
			if ((flock[i]->getPosition() - birdj->getPosition()).norm() < 0.8) {
				pcJ = pcJ + flock[i]->getPosition();	//add the position of that boid to the pcJ vector
			}
		}
	}

	//compute the average of pcJ
	pcJ = pcJ / (flock.size() - 1);

	//determine the new position that corresponds to 1% of the distance from the calling boid to the perceived center
	return (pcJ - birdj->getPosition()) / 100;
}

//Rule 2: Boids try to keep a small distance away from other objects (including other boids)
Eigen::Vector2f Flock::rule2(Bird *birdj) {

	//create a vector 'c' that will act as an offset to displace the boid away from another one
	Eigen::Vector2f c;
	c(0) = 0;
	c(1) = 0;

	//iterate through all the birds in the flock
	for (int i = 0; i<flock.size(); i++) {
		if (flock[i] != birdj) {													//continues only if it is not the same bird
			if ((flock[i]->getPosition() - birdj->getPosition()).norm() < 0.8) {
				if ((flock[i]->getPosition() - birdj->getPosition()).norm() < 0.006) {	//checks to see if the distance is less than 0.05, which is the lowest before overlapping is seen
					c = c - (flock[i]->getPosition() - birdj->getPosition());			//adds the difference in position to the c vector	
				}
			}
		}

	}

	return c;																		//boid will shift in the aggregated offset
}

//Rule 3: Boids try to match velocity with near boids
Eigen::Vector2f Flock::rule3(Bird *birdj) {

	//Create vector pvJ that will hold the average velocity of all other boids
	Eigen::Vector2f pvJ; 
	pvJ(0) = 0;
	pvJ(1) = 0;

	//iterate through each boid
	for (int i = 0; i <flock.size(); i++) {
		if (flock[i] != birdj) {					//continues only if it is not the calling boid
			if ((flock[i]->getPosition() - birdj->getPosition()).norm() < 0.8) {
				pvJ = pvJ + flock[i]->getVelocity();	//adds that boids velocity to the vector
			}
		}
	}

	pvJ = pvJ / (flock.size() - 1);					//compute the average velocity of all other boids

	return (pvJ - birdj->getVelocity()) / 8;		//calling boid's velocity is adjusted by 1/8th of the averge

}

//Tweak to limit the speed of the birds
void Flock::limitSpeed(Bird *bird) {
	float vlim = 0.2;

	//Initially set the velocity to be the current velocity
	Eigen::Vector2f newVelocity = bird->getVelocity();

	//Velocity adjusted only after the bird's velocity surpasses the limit
	if (bird->getVelocity().norm() > vlim) {
		newVelocity(0) = ((bird->getVelocity()(0) / bird->getVelocity().norm()) * vlim);
		newVelocity(1) = ((bird->getVelocity()(1) / bird->getVelocity().norm()) * vlim);
	}
	
	bird->setVelocity(newVelocity);
}
void Flock::boundPosition(Bird *bird) {
	if (bird->getPosition()(0) < -1 + 0.01) {
		Eigen::Vector2f temp;
		temp(0) = 0.01;
		temp(1) = bird->getVelocity()(1);
		bird->setVelocity(temp);
	}
	if (bird->getPosition()(1) < -1 + 0.01) {
		Eigen::Vector2f temp;
		temp(1) = 0.01;
		temp(0) = bird->getVelocity()(0);
		bird->setVelocity(temp);
	}
	if (bird->getPosition()(0) > 1 - 0.01) {
		Eigen::Vector2f temp;
		temp(0) = -0.01;
		temp(1) = bird->getVelocity()(1);
		bird->setVelocity(temp);
	}
	if (bird->getPosition()(1) > 1 - 0.01) {
		Eigen::Vector2f temp;
		temp(1) = -0.01;
		temp(0) = bird->getVelocity()(1);
		bird->setVelocity(temp);
	}
	
}
//Initializing the thread, bird will take a vector as an argument as pass it to its position
void Flock::init_thread(Bird* bird, Eigen::Vector2f newPosition) {

	bird->setPosition(newPosition);

}
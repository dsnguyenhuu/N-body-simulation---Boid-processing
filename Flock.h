//Dong-Son Nguyen-Huu - 40014054
//COMP 426 
//October 8, 2017
//Assignment 1
#pragma once

#include "Bird.h"
#include <vector>
#include <Eigen\Dense>
#include <thread>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

class Flock {
	static size_t global_item_size;
	static size_t local_item_size;

	static cl_platform_id platform_id;
	static cl_device_id device_id;
	static cl_uint ret_num_devices;
	static cl_uint ret_num_platforms;

	static cl_int ret;
	static cl_context context;
	static cl_program program;

	static cl_kernel velocity_kernel;
	static cl_kernel position_kernel;
	static cl_command_queue command_queue;

	static cl_mem dev_old_velocity;
	static cl_mem dev_cohesion;
	static cl_mem dev_separation;
	static cl_mem dev_alignment;
	static cl_mem dev_new_velocity;
	static cl_mem dev_old_position;
	static cl_mem dev_new_position;

private:
	std::vector<Bird*> flock;
	static float color;
	std::vector<std::thread*> thread;

public:
	Flock(int numBird);

	const std::vector<Bird*> &getFlock() const;
	void updateBird();

	Eigen::Vector2f rule1(Bird *bj);
	Eigen::Vector2f rule2(Bird *bj);
	Eigen::Vector2f rule3(Bird *bj);

	void boundPosition(Bird *bj);


	void limitSpeed(Bird *bj);
	static void init_thread(Bird* bird, Eigen::Vector2f newPosition);
	
};

__kernel void computeNewVelocity(__global const float *old_velocity, __global const float *cohesion, __global float *separation, __global float *alignment, __global float *new_velocity) {

	// Get the index of the current element to be processed
	int i = get_global_id(0);

	// Do the operation
	new_velocity[i] = old_velocity[i] + cohesion[i] + separation[i] + alignment[i];
}

__kernel void computeNewPosition(__global const float *old_position, __global float *new_velocity, __global float *new_position) {

	// Get the index of the current element to be processed
	int i = get_global_id(0);

	// Do the operation
	new_position[i] = old_position[i] + new_velocity[i];
}
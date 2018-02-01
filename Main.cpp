//Dong-Son Nguyen-Huu - 40014054
//COMP 426 
//October 8, 2017
//Assignment 1

#include "Start.h"
#define N 25

Flock flock1(N);
Flock flock2(N);
Flock flock3(N);
Flock flock4(N);
Flock flock5(N);
Flock flock6(N);
Flock flock7(N);
//Flock flock8(N);
//Flock flock9(N);
//Flock flock10(N);
//Flock flock11(N);
//Flock flock12(N);
//Flock flock13(N);
//Flock flock14(N);
//Flock flock15(N);
//Flock flock16(N);
//Flock flock17(N);
//Flock flock18(N);
//Flock flock19(N);
//Flock flock20(N);
//


int main() {
	//std::vector <Flock> flocks{ flock1,flock2, flock3, flock4, flock5, flock6, flock7, flock8, flock9, flock10,flock11,flock12,flock13,flock14,flock15,flock16,flock17,flock18,flock19,flock20};
	
	//initialize the main class that will run the algorithm
	Start flight;
	flight.initialize("Assignment 3 - Boid Algorithm");


	std::vector <Flock> flocks{ flock1,flock2,flock3, flock4, flock5, flock6 };

	while (true) {
		//controls the computations, to be done every 1/30th of a second
		flight.update();

		//calculates the boids' new positions and displays them as well
		flight.render(flocks);
	}
}
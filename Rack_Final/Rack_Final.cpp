//Rack_Final.cpp: main driver file for the rack algorithm. Reads in data, initializes an
//instance of the Program class, executes the algorithm, and displays results

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include "..\Program.h"

using namespace std;

int main() {
	Program my_program;

	//read and initialize
	my_program.read_data();
	my_program.populate_frequencies();

	//execute algorithm
	my_program.distribute_racks();

	//display results
	my_program.print_summary();
	my_program.export_results();
}

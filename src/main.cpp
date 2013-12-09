#include <iostream>

#include "Buddha.h"

int main(int argc, char * argv[]) {

	try {
		auto params = Buddha::get_empty_params();
		params.width = 512;
		params.radius = 2;
		params.max_iterations = 20;

		Buddha b(params);
		b.run();
	} catch (std::exception & e) {
		std::cerr << "EXCEPTION : " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "UNKNOWN EXCEPTION" << std::endl;
	}

	return 0;
}

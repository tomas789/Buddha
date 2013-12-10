#include <iostream>

#include "Buddha.h"
#include "ConfigLoader.h"

int main(int argc, char * argv[]) {
    try {
        if (1 == argc) {
            auto params = Buddha::get_empty_params();

            params.width = 512;
            params.radius = 2;
            params.max_iterations = 20;
            params.min_iterations = 5;
            params.subpixel_resolution = 3;

            Buddha b(params);
            b.run();

        } else if (2 == argc) {

            std::vector<ConfigLoader::param_type> ps = ConfigLoader::load(argv[1]);
            for (auto& p : ps) {
                Buddha b(p);
                b.run();
            }

        } else {
            // TODO: Show usage
            std::cerr << "Wrong number of arguments" << std::endl;
            return 1;
        }

    } catch (std::exception & e) {
        std::cerr << "EXCEPTION : " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
    }

    return 0;
}

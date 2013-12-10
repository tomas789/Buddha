#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "ConfigLoader.h"
#include "Buddha.h"

void ConfigLoader::report_error(std::string error, std::string code, std::size_t pos) {
    std::cerr << "ERROR: " << error << std::endl
              << code << std::endl
              << std::string(pos != 0 ? pos - 1 : 0, ' ') << '^' << std::endl;
}

void ParsingConfigFileException::regenerate() {
    /* is there anythink to print */
    if (!msg_.empty())
        what_ = msg_;

    /* Do I have source code to highlight */
    if (line_.empty())
        return;

    /* Should I show filename or line number */
    std::string file_ptr;
    if (!filename_.empty()) {
        file_ptr = filename_;
        if (line_number_ != -1) {
            file_ptr += ":" + std::to_string(line_number_);
        }
    }

    what_ += "\n" + file_ptr + " " + line_;

    /* Highlight char */
    if (pos_ != -1) {
        what_ += "\n";
        auto point_at = file_ptr.size() == 0 ? 0 : (file_ptr.size() + 1) + pos_;
        what_ += std::string(point_at != 0 ? point_at : 0, ' ');
        what_ += "^";
    }

}

void ParsingConfigFileException::set_file(std::string filename, int line_number) {
    filename_ = filename;
    line_number_ = line_number;

    regenerate();
}

void ParsingConfigFileException::set_error_message(std::string msg) {
    msg_ = msg;

    regenerate();
}

void ParsingConfigFileException::set_line(std::string line, int pos) {
    line_ = line;
    pos_ = pos;

    regenerate();
}

const char * ParsingConfigFileException::what() const noexcept {
    return what_.c_str();
}

std::vector<ConfigLoader::param_type> ConfigLoader::load(std::string filename) {
    std::map<std::string, param_type> p;
    std::ifstream in(filename);

    if (!in.is_open())
        throw UnableOpenConfigFileException();

    std::string line;
    std::string section_name;

    for (int ln = 0; std::getline(in, line); ++ln) {

        if (line.empty()) continue;
        switch(line[0]) {
            case '#':
            case ';':
                continue;
        }

        auto sep = line.find_first_of("=");
        
        if (std::string::npos == sep) {
            // Possibly section separator
            if ('[' != line.front()) {
                auto e = ParsingConfigFileException();
                e.set_file(filename, ln + 1);
                e.set_error_message("Expected '['");
                e.set_line(line, 0);
                throw e;
            }

            auto end = line.find_last_not_of(" \t");
            if (']' != line[end]) {
                auto e = ParsingConfigFileException();
                e.set_file(filename, ln + 1);
                e.set_error_message("Expected ']'");
                e.set_line(line, end);
                throw e;
            }

            section_name = line.substr(1, end);
            if (p.find(section_name) == p.end())
                p[section_name] = Buddha::get_empty_params();

        } else {
            // Parameter
            std::string key = trim(line.substr(0, sep));
            std::string value = trim(line.substr(sep + 1, std::string::npos));

            if ("name" == key)
                p[section_name].name = value;
            else if ("format" == key)
                p[section_name].format = value;
            else if ("width" == key) {
                try {
                    p[section_name].width = std::stoul(value);
                } catch(std::exception exp) {
                    ParsingConfigFileException e;
                    e.set_file(filename, ln + 1);
                    e.set_error_message("Unable parse '" + value + ": " + exp.what());
                    throw e;
                }
            } else if ("radius" == key) {
                std::istringstream oss(value);
                if (!(oss >> p[section_name].radius)) {
                    ParsingConfigFileException e;
                    e.set_file(filename, ln + 1);
                    e.set_error_message("Unable parse as double: " + value);
                    throw e;
                }
            } else if ("max iterations" == key) {
                std::istringstream oss(value);
                if (!(oss >> p[section_name].max_iterations)) {
                    ParsingConfigFileException e;
                    e.set_file(filename, ln + 1);
                    e.set_error_message("Unable parse as double: " + value);
                    throw e;
                }
            } else if ("min iterations" == key) {
                std::istringstream oss(value);
                if (!(oss >> p[section_name].min_iterations)) {
                    ParsingConfigFileException e;
                    e.set_file(filename, ln + 1);
                    e.set_error_message("Unable parse as double: " + value);
                    throw e;
                }
            } else if ("subpixel resolution" == key) {
                std::istringstream oss(value);
                if (!(oss >> p[section_name].subpixel_resolution)) {
                    ParsingConfigFileException e;
                    e.set_file(filename, ln + 1);
                    e.set_error_message("Unable parse as double: " + value);
                    throw e;
                }
            } else if ("threads" == key) {
                std::istringstream oss(value);
                if (!(oss >> p[section_name].num_threads)) {
                    ParsingConfigFileException e;
                    e.set_file(filename, ln + 1);
                    e.set_error_message("Unable parse as double: " + value);
                    throw e;
                }
            } else {
                ParsingConfigFileException e;
                e.set_file(filename, ln + 1);
                e.set_error_message("Unknown key: " + key);
                throw e;
            }
        }

    }

    std::vector<param_type> ps;
    for (auto & param : p) 
        ps.push_back(param.second);

    return ps;
}

#ifndef _CONFIGLOADER_H
#define _CONFIGLOADER_H

#include <string>
#include <exception>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "Buddha.h"

class UnableOpenConfigFileException : public virtual std::exception { };
class ParsingConfigFileException : public virtual std::exception {
    std::string filename_;
    int line_number_ = -1;
    std::string msg_;
    std::string line_;
    int pos_ = -1;
    std::string what_;

    void regenerate();
public:
    void set_file(std::string filename, int line_number = -1);
    void set_error_message(std::string msg);
    void set_line(std::string line, int pos = -1);
    virtual const char * what() const noexcept;
};

class ConfigLoader {
    static void report_error(std::string error, std::string code = "", std::size_t pos = 0);
public:
    typedef decltype(Buddha::get_empty_params()) param_type;
    static std::vector<param_type> load(std::string filename);

    static inline std::string trim(const std::string & s) {
        auto begin = s.find_first_not_of(" \t");
        auto end = s.find_last_not_of(" \t");
        return s.substr(begin, end == s.size() - 1 
                                   ? std::string::npos 
                                   : end + 1
                       );
    }
};

#endif // _CONFIGLOADER_H

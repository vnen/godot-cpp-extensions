#include "error_macros.hpp"

#include "godot.hpp"

#include <string>

namespace godot {

void _err_print_error(const char *p_function, const char *p_file, int p_line, const char *p_error, const char *p_message, bool p_is_warning) {
    if (p_is_warning) {
        interface->print_warning(p_message, p_function, p_file, p_line);
    } else {
        interface->print_error(p_message, p_function, p_file, p_line);
    }
}

void _err_print_error(const char *p_function, const char *p_file, int p_line, const char *p_error, bool p_is_warning) {
    _err_print_error(p_function, p_file, p_line, p_error, "", p_is_warning);
}

void _err_print_index_error(const char *p_function, const char *p_file, int p_line, int64_t p_index, int64_t p_size, const char *p_index_str, const char *p_size_str, const char *p_message, bool fatal) {
	std::string fstr(fatal ? "FATAL: " : "");
	std::string err(fstr + "Index " + p_index_str + " = " + std::to_string(p_index) + " is out of bounds (" + p_size_str + " = " + std::to_string(p_size) + ").");
	_err_print_error(p_function, p_file, p_line, err.c_str(), p_message);
}

}

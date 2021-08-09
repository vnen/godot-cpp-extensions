#include <variant/char_string.hpp>

#include <core/memory.hpp>
#include <variant/node_path.hpp>
#include <variant/string.hpp>
#include <variant/string_name.hpp>

#include <godot.hpp>

namespace godot {

const char *CharString::get_data() const {
	return _data;
}

CharString::CharString(const char *str, int length) :
		_data(str), _length(length) {}

CharString::~CharString() {
	memdelete_arr(_data);
}

Char16String::Char16String(const char16_t *str, int length) :
		_data(str), _length(length) {}

Char16String::~Char16String() {
	memdelete_arr(_data);
}

Char32String::Char32String(const char32_t *str, int length) :
		_data(str), _length(length) {}

Char32String::~Char32String() {
	memdelete_arr(_data);
}

CharWideString::CharWideString(const wchar_t *str, int length) :
		_data(str), _length(length) {}

CharWideString::~CharWideString() {
	memdelete_arr(_data);
}

// Custom String functions that are not part of bound API.
// It's easier to have them written in C++ directly than in a Python script that generates them.

String::String(const char *from) {
    internal::interface->string_new_with_utf8_chars(ptr, from);
}

String::String(const wchar_t *from) {
    internal::interface->string_new_with_wide_chars(ptr, from);
}

String::String(const char16_t *from) {
    internal::interface->string_new_with_utf16_chars(ptr, from);
}

String::String(const char32_t *from) {
    internal::interface->string_new_with_utf32_chars(ptr, from);
}

CharString String::utf8() const {
    int size = internal::interface->string_to_utf8_chars(ptr, nullptr, 0);
    char *cstr = memnew_arr(char, size + 1);
    internal::interface->string_to_utf8_chars(ptr, cstr, size + 1);

    cstr[size] = '\0';

    return CharString(cstr, size + 1);
}

CharString String::ascii() const {
    int size = internal::interface->string_to_latin1_chars(ptr, nullptr, 0);
    char *cstr = memnew_arr(char, size + 1);
    internal::interface->string_to_latin1_chars(ptr, cstr, size + 1);

    cstr[size] = '\0';

    return CharString(cstr, size + 1);
}

Char16String String::utf16() const {
    int size = internal::interface->string_to_utf16_chars(ptr, nullptr, 0);
    char16_t *cstr = memnew_arr(char16_t, size + 1);
    internal::interface->string_to_utf16_chars(ptr, cstr, size + 1);

    cstr[size] = '\0';

    return Char16String(cstr, size + 1);
}

Char32String String::utf32() const {
    int size = internal::interface->string_to_utf32_chars(ptr, nullptr, 0);
    char32_t *cstr = memnew_arr(char32_t, size + 1);
    internal::interface->string_to_utf32_chars(ptr, cstr, size + 1);

    cstr[size] = '\0';

    return Char32String(cstr, size + 1);
}

CharWideString String::wide_string() const {
    int size = internal::interface->string_to_wide_chars(ptr, nullptr, 0);
    wchar_t *cstr = memnew_arr(wchar_t, size + 1);
    internal::interface->string_to_wide_chars(ptr, cstr, size + 1);

    cstr[size] = '\0';

    return CharWideString(cstr, size + 1);
}

StringName::StringName(const char *from) : StringName(String(from)) {}

StringName::StringName(const wchar_t *from) : StringName(String(from)) {}

StringName::StringName(const char16_t *from) : StringName(String(from)) {}

StringName::StringName(const char32_t *from) : StringName(String(from)) {}

NodePath::NodePath(const char *from) : NodePath(String(from)) {}

NodePath::NodePath(const wchar_t *from) : NodePath(String(from)) {}

NodePath::NodePath(const char16_t *from) : NodePath(String(from)) {}

NodePath::NodePath(const char32_t *from) : NodePath(String(from)) {}

} // namespace godot

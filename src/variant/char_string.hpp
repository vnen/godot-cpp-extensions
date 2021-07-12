#ifndef GODOT_CPP_CHAR_STRING_HPP
#define GODOT_CPP_CHAR_STRING_HPP

#include <cstddef>
#include <cstdint>

namespace godot {

class CharString {
    friend class String;

    const char *_data = nullptr;
    int _length = 0;

    CharString(const char *str, int length);

public:
    int length() const;
    const char *get_data() const;

    ~CharString();
};

class Char16String {
    friend class String;

    const char16_t *_data = nullptr;
    int _length = 0;

    Char16String(const char16_t *str, int length);

public:
    int length() const;
    const char16_t *get_data() const;

    ~Char16String();
};

class Char32String {
    friend class String;

    const char32_t *_data = nullptr;
    int _length = 0;

    Char32String(const char32_t *str, int length);

public:
    int length() const;
    const char32_t *get_data() const;

    ~Char32String();
};

class CharWideString {
    friend class String;

    const wchar_t *_data = nullptr;
    int _length = 0;

    CharWideString(const wchar_t *str, int length);

public:
    int length() const;
    const wchar_t *get_data() const;

    ~CharWideString();
};

}

#endif // ! GODOT_CPP_CHAR_STRING_HPP

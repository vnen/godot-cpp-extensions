project('sample-gdnative', 'c', 'cpp',
    default_options : ['c_std=c99', 'cpp_std=c++17'])

include_path = include_directories('.', 'src')

lib_sources = [
    'src/godot.cpp',
    'src/core/class_db.cpp',
    'src/core/error_macros.cpp',
    'src/core/memory.cpp',
    'src/core/method_bind.cpp',
    'src/core/object.cpp',
    'src/gen/engine_classes.cpp',
]

user_sources = [
    'sample-init.cpp',
    'sample-script.cpp',
]

defines = [
    '-DDEBUG_ENABLED',
]

godot_cpp_lib = static_library('godot-cpp', sources: lib_sources, include_directories: include_path, cpp_args: defines)
shared_library('sample_lib', user_sources, include_directories: include_path, cpp_args: defines, link_with: godot_cpp_lib)

project('sample-gdnative', 'c', 'cpp',
    default_options : ['c_std=c99', 'cpp_std=c++17'])

include_path = include_directories('.', 'src')

sources = [
    'src/godot.cpp',
    'src/core/class_db.cpp',
    'src/core/error_macros.cpp',
    'src/core/memory.cpp',
    'src/core/method_bind.cpp',
    'src/core/object.cpp',
    'src/gen/engine_classes.cpp',
    'sample-init.cpp',
    'sample-script.cpp',
]

defines = [
    '-DDEBUG_ENABLED',
]

shared_library('sample_lib', sources, include_directories: include_path, cpp_args: defines)

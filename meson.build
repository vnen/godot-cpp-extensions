project('sample-gdnative', 'c', 'cpp',
    default_options : ['c_std=c99', 'cpp_std=c++17'])

include_path = include_directories('.', 'src', 'gen/include')

lib_sources = [
    'src/godot.cpp',
    'src/core/class_db.cpp',
    'src/core/error_macros.cpp',
    'src/core/memory.cpp',
    'src/core/method_bind.cpp',
    'src/core/object.cpp',
    'src/variant/variant.cpp',
    # 'src/gen/core_types.cpp',
    # 'src/gen/engine_classes.cpp',
    'src/variant/char_string.cpp',
]

user_sources = [
    'sample-init.cpp',
    'sample-script.cpp',
]

defines = [
    '-DDEBUG_ENABLED',
]

compiler_args = [
    # '-fsanitize=address,pointer-subtract,pointer-compare',
]

linker_args = [
    # '-fsanitize=address',
]

glob_cmd = run_command('add_gen_files.py')
gen_sources = glob_cmd.stdout().strip().split('\n')

godot_cpp_lib = static_library('godot-cpp', sources: lib_sources + gen_sources, include_directories: include_path, cpp_args: defines + compiler_args, link_args: linker_args)
shared_library('sample_lib', user_sources, include_directories: include_path, cpp_args: defines + compiler_args, link_args: linker_args, link_with: godot_cpp_lib)

#!/usr/bin/env python

import json
import re
import shutil
from pathlib import Path


def generate_bindings(api_filepath, use_template_get_node, output_dir="gen"):
    api = None

    with open(api_filepath) as api_file:
        api = json.load(api_file)

    generate_builtin_bindings(api, "float_64", output_dir)


def generate_builtin_bindings(api, build_config, output_dir):
    include_gen_folder = Path(output_dir) / "include" / "variant"
    source_gen_folder = Path(output_dir) / "src" / "variant"

    shutil.rmtree(include_gen_folder, ignore_errors=True)
    shutil.rmtree(source_gen_folder, ignore_errors=True)

    include_gen_folder.mkdir(parents=True)
    source_gen_folder.mkdir(parents=True)

    builtin_sizes = {}

    for size_list in api["builtin_class_sizes"]:
        if size_list["build_configuration"] == build_config:
            for size in size_list["sizes"]:
                builtin_sizes[size["name"]] = size["size"]
            break

    builtin_classes = []

    for builtin_api in api["builtin_classes"]:
        if is_pod_type(builtin_api["name"]):
            continue

        builtin_classes.append(builtin_api["name"])

        size = builtin_sizes[builtin_api["name"]]

        header_filename = include_gen_folder / (
            camel_to_snake(builtin_api["name"]) + ".hpp"
        )
        source_filename = source_gen_folder / (
            camel_to_snake(builtin_api["name"]) + ".cpp"
        )

        # Check used classes for header include
        used_classes = set()
        fully_used_classes = set()

        class_name = builtin_api["name"]

        if "constructors" in builtin_api:
            for constructor in builtin_api["constructors"]:
                if "arguments" in constructor:
                    for argument in constructor["arguments"]:
                        if is_included(argument["type"], class_name):
                            if "default_value" in argument:
                                fully_used_classes.add(argument["type"])
                            else:
                                used_classes.add(argument["type"])

        if "methods" in builtin_api:
            for method in builtin_api["methods"]:
                if "arguments" in method:
                    for argument in method["arguments"]:
                        if is_included(argument["type"], class_name):
                            if "default_value" in argument:
                                fully_used_classes.add(argument["type"])
                            else:
                                used_classes.add(argument["type"])
                if "return_type" in method:
                    if is_included(method["return_type"], class_name):
                        used_classes.add(method["return_type"])

        if "members" in builtin_api:
            for member in builtin_api["members"]:
                if is_included(member["type"], class_name):
                    used_classes.add(member["type"])

        if "indexing_return_type" in builtin_api:
            if is_included(builtin_api["indexing_return_type"], class_name):
                used_classes.add(builtin_api["indexing_return_type"])

        if "operators" in builtin_api:
            for operator in builtin_api["operators"]:
                if "right_type" in operator:
                    if is_included(operator["right_type"], class_name):
                        used_classes.add(operator["right_type"])

        for type_name in fully_used_classes:
            if type_name in used_classes:
                used_classes.remove(type_name)

        with header_filename.open("w+") as header_file:
            header_file.write(
                generate_builtin_class_header(
                    builtin_api, size, used_classes, fully_used_classes
                )
            )

        with source_filename.open("w+") as source_file:
            source_file.write(
                generate_builtin_class_source(
                    builtin_api, size, used_classes, fully_used_classes
                )
            )

    # Create a header with all builtin types for convenience.
    builtin_header_filename = include_gen_folder / "builtin_types.hpp"
    with builtin_header_filename.open("w+") as builtin_header_file:
        builtin_header = ["// THIS FILE IS GENERATED. EDITS WILL BE LOST."]
        builtin_header.append("#ifndef GODOT_CPP_BUILTIN_TYPES_HPP")
        builtin_header.append("#define GODOT_CPP_BUILTIN_TYPES_HPP")

        builtin_header.append("")

        for builtin in builtin_classes:
            builtin_header.append(f"#include \"{camel_to_snake(builtin)}.hpp\"")

        builtin_header.append("")

        builtin_header.append("#endif // ! GODOT_CPP_BUILTIN_TYPES_HPP")

        builtin_header_file.write("\n".join(builtin_header))


def generate_builtin_class_header(builtin_api, size, used_classes, fully_used_classes):
    result = ["// THIS FILE IS GENERATED. EDITS WILL BE LOST."]

    class_name = builtin_api["name"]
    snake_class_name = camel_to_snake(class_name).upper()

    header_guard = f"GODOT_CPP_{snake_class_name}_HPP"

    result.append(f"#ifndef {header_guard}")
    result.append(f"#define {header_guard}")

    result.append("")
    # result.append('#include "builtin_types.hpp"')
    result.append('#include "core/defs.hpp"')
    result.append("")

    for include in fully_used_classes:
        result.append(
            f'#include "{"core/" if include == "Object" else "variant/"}{camel_to_snake(include)}.hpp"'
        )

    if len(fully_used_classes) > 0:
        result.append("")

    result.append("#include <godot-headers/gdnative_interface.h>")
    result.append("")
    result.append("namespace godot {")
    result.append("")

    for type_name in used_classes:
        result.append(f"class {type_name};")

    if len(used_classes) > 0:
        result.append("")

    result.append(f"class {class_name} {{")
    result.append(f"\tstatic constexpr size_t {snake_class_name}_SIZE = {size};")
    result.append(f"\tuint8_t opaque[{snake_class_name}_SIZE] {{ 0 }};")

    result.append("")
    result.append("\tfriend class Variant;")

    result.append("")
    result.append("\tstatic struct _MethodBindings {")

    if "constructors" in builtin_api:
        for constructor in builtin_api["constructors"]:
            result.append(
                f'\t\tGDNativePtrConstructor constructor_{constructor["index"]};'
            )

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            result.append(f'\t\tGDNativePtrBuiltInMethod method_{method["name"]};')

    if "members" in builtin_api:
        for member in builtin_api["members"]:
            result.append(f'\t\tGDNativePtrSetter member_{member["name"]}_setter;')
            result.append(f'\t\tGDNativePtrGetter member_{member["name"]}_getter;')

    if "indexing_return_type" in builtin_api:
        result.append(f"\t\tGDNativePtrIndexedSetter indexed_setter;")
        result.append(f"\t\tGDNativePtrIndexedGetter indexed_getter;")

    if "is_keyed" in builtin_api and builtin_api["is_keyed"]:
        result.append(f"\t\tGDNativePtrKeyedSetter keyed_setter;")
        result.append(f"\t\tGDNativePtrKeyedGetter keyed_getter;")
        result.append(f"\t\tGDNativePtrKeyedChecker keyed_checker;")

    if "operators" in builtin_api:
        for operator in builtin_api["operators"]:
            if "right_type" in operator:
                result.append(
                    f'\t\tGDNativePtrOperatorEvaluator operator_{get_operator_id_name(operator["name"])}_{operator["right_type"]};'
                )
            else:
                result.append(
                    f'\t\tGDNativePtrOperatorEvaluator operator_{get_operator_id_name(operator["name"])};'
                )

    result.append("\t} _method_bindings;")

    result.append("")
    result.append("\tstatic void init_bindings();")

    result.append("")
    result.append("public:")

    if "constructors" in builtin_api:
        for constructor in builtin_api["constructors"]:
            method_signature = f"\t{class_name}("
            if "arguments" in constructor:
                method_signature += make_function_parameters(
                    constructor["arguments"], include_default=True
                )
            method_signature += ");"

            result.append(method_signature)

    # Special cases.
    if class_name == "String":
        result.append("\tString(const char *from);")
        result.append("\tString(const wchar_t *from);")
        result.append("\tString(const char16_t *from);")
        result.append("\tString(const char32_t *from);")

    method_list = []

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            method_list.append(method["name"])
            method_signature = "\t"
            if "return_type" in method:
                method_signature += f'{method["return_type"]} '
            else:
                method_signature += "void "

            method_signature += f'{method["name"]}('
            if "arguments" in method:
                method_signature += make_function_parameters(
                    method["arguments"], include_default=True
                )
            method_signature += ")"
            if method["is_const"]:
                method_signature += " const"
            method_signature += ";"

            result.append(method_signature)

    if "members" in builtin_api:
        for member in builtin_api["members"]:
            if f'get_{member["name"]}' not in method_list:
                result.append(
                    f'\t{correct_type(member["type"])} get_{member["name"]}() const;'
                )
            if f'set_{member["name"]}' not in method_list:
                result.append(
                    f'\tvoid set_{member["name"]}({"const " + correct_type(member["type"]) + " &" if not is_pod_type(member["type"]) else correct_type(member["type"]) + " "}value);'
                )

    result.append("};")

    result.append("")
    result.append("} // namespace godot")

    result.append(f"#endif // ! {header_guard}")

    return "\n".join(result)


def generate_builtin_class_source(builtin_api, size, used_classes, fully_used_classes):
    result = ["// THIS FILE IS GENERATED. EDITS WILL BE LOST."]

    class_name = builtin_api["name"]
    snake_class_name = camel_to_snake(class_name)
    enum_type_name = f"GDNATIVE_VARIANT_TYPE_{snake_class_name.upper()}"

    result.append("")
    result.append(f'#include "variant/{snake_class_name}.hpp"')
    result.append("")
    result.append('#include "godot.hpp"')
    result.append("")

    # Only used since the "fully used" is included in header already.
    for include in used_classes:
        result.append(
            f'#include "{"core/" if include == "Object" else "variant/"}{camel_to_snake(include)}.hpp"'
        )

    if len(used_classes) > 0:
        result.append("")

    result.append('#include "core/builtin_ptrcall.hpp"')
    result.append("")
    result.append("namespace godot {")
    result.append("")

    result.append(f"{class_name}::_MethodBindings {class_name}::_method_bindings;")
    result.append("")

    result.append(f"void {class_name}::init_bindings() {{")

    if "constructors" in builtin_api:
        for constructor in builtin_api["constructors"]:
            result.append(
                f'\t_method_bindings.constructor_{constructor["index"]} = interface->variant_get_ptr_constructor({enum_type_name}, {constructor["index"]});'
            )

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            # TODO: Add error check for hash mismatch.
            result.append(
                f'\t_method_bindings.method_{method["name"]} = interface->variant_get_ptr_builtin_method({enum_type_name}, "{method["name"]}", {method["hash"]});'
            )

    if "members" in builtin_api:
        for member in builtin_api["members"]:
            result.append(
                f'\t_method_bindings.member_{member["name"]}_setter = interface->variant_get_ptr_setter({enum_type_name}, "{member["name"]}");'
            )
            result.append(
                f'\t_method_bindings.member_{member["name"]}_getter = interface->variant_get_ptr_getter({enum_type_name}, "{member["name"]}");'
            )

    if "indexing_return_type" in builtin_api:
        result.append(
            f"\t_method_bindings.indexed_setter = interface->variant_get_ptr_indexed_setter({enum_type_name});"
        )
        result.append(
            f"\t_method_bindings.indexed_getter = interface->variant_get_ptr_indexed_getter({enum_type_name});"
        )

    if "is_keyed" in builtin_api and builtin_api["is_keyed"]:
        result.append(
            f"\t_method_bindings.keyed_setter = interface->variant_get_ptr_keyed_setter({enum_type_name});"
        )
        result.append(
            f"\t_method_bindings.keyed_getter = interface->variant_get_ptr_keyed_getter({enum_type_name});"
        )
        result.append(
            f"\t_method_bindings.keyed_checker = interface->variant_get_ptr_keyed_checker({enum_type_name});"
        )

    if "operators" in builtin_api:
        for operator in builtin_api["operators"]:
            if "right_type" in operator:
                result.append(
                    f'\t_method_bindings.operator_{get_operator_id_name(operator["name"])}_{operator["right_type"]} = interface->variant_get_ptr_operator_evaluator(GDNATIVE_VARIANT_OP_{get_operator_id_name(operator["name"]).upper()}, {enum_type_name}, GDNATIVE_VARIANT_TYPE_{camel_to_snake(operator["right_type"]).upper()});'
                )
            else:
                result.append(
                    f'\t_method_bindings.operator_{get_operator_id_name(operator["name"])} = interface->variant_get_ptr_operator_evaluator(GDNATIVE_VARIANT_OP_{get_operator_id_name(operator["name"]).upper()}, {enum_type_name}, GDNATIVE_VARIANT_TYPE_NIL);'
                )

    result.append("}")

    if "constructors" in builtin_api:
        for constructor in builtin_api["constructors"]:
            method_signature = f"{class_name}::{class_name}("
            if "arguments" in constructor:
                method_signature += make_function_parameters(
                    constructor["arguments"], include_default=False
                )
            method_signature += ") {"

            result.append(method_signature)

            method_call = f'\tinternal::_call_builtin_constructor(_method_bindings.constructor_{constructor["index"]}, &opaque'
            if "arguments" in constructor:
                method_call += ", "
                arguments = []
                for argument in constructor["arguments"]:
                    arguments.append(escape_identifier(argument["name"]))
                method_call += ", ".join(arguments)
            method_call += ");"

            result.append(method_call)
            result.append("}")

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            method_signature = ""
            if "return_type" in method:
                method_signature += f'{method["return_type"]} '
            else:
                method_signature += "void "

            method_signature += f'{class_name}::{method["name"]}('
            if "arguments" in method:
                method_signature += make_function_parameters(
                    method["arguments"], include_default=False
                )
            method_signature += ")"
            if method["is_const"]:
                method_signature += " const"
            method_signature += " {"

            result.append(method_signature)

            method_call = "\t"
            if "return_type" in method:
                method_call += f'return internal::_call_builtin_method_ptr_ret<{method["return_type"]}>('
            else:
                method_call += f"internal::_call_builtin_method_ptr_no_ret("
            method_call += (
                f'_method_bindings.method_{method["name"]}, (GDNativeTypePtr)&opaque'
            )
            if "arguments" in method:
                arguments = []
                method_call += ", "
                for argument in method["arguments"]:
                    arguments.append(escape_identifier(argument["name"]))
                method_call += ", ".join(arguments)
            method_call += ");"

            result.append(method_call)
            result.append("}")

    result.append("")
    result.append("} //namespace godot")

    return "\n".join(result)


# Helper functions.


def camel_to_snake(name):
    name = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", name)
    name = re.sub("([a-z0-9])([A-Z])", r"\1_\2", name)
    return name.replace("2_D", "2D").replace("3_D", "3D").lower()


def make_function_parameters(parameters, include_default=False):
    signature = []

    for par in parameters:
        parameter = ""
        if is_pod_type(par["type"]):
            parameter += f'{correct_type(par["type"])} '
        else:
            parameter += f'const {correct_type(par["type"])} &'
        parameter += f'{escape_identifier(par["name"])}'

        if include_default and "default_value" in par:
            parameter += f' = {correct_default_value(par["default_value"])}'
        signature.append(parameter)

    return ", ".join(signature)


# Engine idiosyncrasies.


def is_pod_type(type_name):
    """
    Those are types for which no class should be generated.
    """
    return type_name in [
        "Nil",
        "int",
        "float",
        "bool",
    ]


def is_included(type_name, current_type):
    """
    Check if a builtin type should be included.
    This removes Variant and POD types from inclusion, and the current type.
    """
    return type_name != current_type and not is_pod_type(type_name)


def correct_default_value(value):
    if value == "null":
        return "nullptr"
    elif value == '""':
        return "String()"
    if value == "[]":
        return "Array()"
    if value == "{}":
        return "Dictionary()"
    return value


type_conversion = {"float": "double", "int": "int64_t"}


def correct_type(type_name):
    if type_name in type_conversion:
        return type_conversion[type_name]
    return type_name


cpp_keywords = [
    "char",
    "default",
]


def escape_identifier(id):
    if id in cpp_keywords:
        return "_" + id
    return id


op_id_map = {
    "==": "equal",
    "!=": "not_equal",
    "<": "less",
    "<=": "less_equal",
    ">": "greater",
    ">=": "greater_equal",
    "+": "add",
    "-": "subtract",
    "*": "multiply",
    "/": "divide",
    "unary-": "negate",
    "unary+": "positive",
    "%": "module",
    "<<": "shift_left",
    ">>": "shift_right",
    "&": "bit_and",
    "|": "bit_or",
    "^": "bit_xor",
    "~": "bit_negate",
    "and": "and",
    "or": "or",
    "xor": "xor",
    "not": "not",
    "and": "and",
    "in": "in",
}


def get_operator_id_name(op):
    return op_id_map[op]


# Just for debugging.
if __name__ == "__main__":
    generate_bindings("godot-headers/extension_api.json", True)

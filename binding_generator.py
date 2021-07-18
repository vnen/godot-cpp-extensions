#!/usr/bin/env python

import json
import re
import shutil
from pathlib import Path


def generate_bindings(api_filepath, use_template_get_node, output_dir="gen"):
    api = None

    with open(api_filepath) as api_file:
        api = json.load(api_file)

    shutil.rmtree(output_dir, ignore_errors=True)
    Path(output_dir).mkdir(parents=True)

    generate_global_constants(api, output_dir)
    generate_builtin_bindings(api, output_dir, "float_64")
    generate_engine_classes_bindings(api, output_dir, use_template_get_node)


builtin_classes = []

# Key is class name, value is boolean where True means the class is refcounted.
engine_classes = {}


def generate_builtin_bindings(api, output_dir, build_config):
    global builtin_classes

    include_gen_folder = Path(output_dir) / "include" / "variant"
    source_gen_folder = Path(output_dir) / "src" / "variant"

    include_gen_folder.mkdir(parents=True, exist_ok=True)
    source_gen_folder.mkdir(parents=True, exist_ok=True)

    # Store types beforehand.
    for builtin_api in api["builtin_classes"]:
        if is_pod_type(builtin_api["name"]):
            continue
        builtin_classes.append(builtin_api["name"])

    builtin_sizes = {}

    for size_list in api["builtin_class_sizes"]:
        if size_list["build_configuration"] == build_config:
            for size in size_list["sizes"]:
                builtin_sizes[size["name"]] = size["size"]
            break

    for builtin_api in api["builtin_classes"]:
        if is_pod_type(builtin_api["name"]):
            continue

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
            builtin_header.append(f'#include "{camel_to_snake(builtin)}.hpp"')

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
    result.append('#include "core/defs.hpp"')
    result.append("")

    # Special cases.
    if class_name == "String":
        result.append("#include <variant/char_string.hpp>")

    for include in fully_used_classes:
        result.append(f"#include <{get_include_path(include)}>")

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

    if builtin_api["has_destructor"]:
        result.append("\t\tGDNativePtrDestructor destructor;")

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

    if "constants" in builtin_api:
        axis_constants_count = 0
        for constant in builtin_api["constants"]:
            # Special case: Vector3.Axis is the only enum in the bindings.
            # It's technically not supported by Variant but works for direct access.
            if class_name == "Vector3" and constant["name"].startswith("AXIS"):
                if axis_constants_count == 0:
                    result.append("\tenum Axis {")
                result.append(f'\t\t{constant["name"]} = {constant["value"]},')
                axis_constants_count += 1
                if axis_constants_count == 3:
                    result.append("\t};")
            else:
                result.append(
                    f'\tstatic const {correct_type(constant["type"])} {constant["name"]};'
                )

    if builtin_api["has_destructor"]:
        result.append(f"\t~{class_name}();")

    method_list = []

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            method_list.append(method["name"])
            method_signature = "\t"
            if "return_type" in method:
                method_signature += f'{correct_type(method["return_type"])} '
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

    # Special cases.
    if class_name == "String":
        result.append("\tCharString utf8() const;")
        result.append("\tCharString ascii() const;")
        result.append("\tChar16String utf16() const;")
        result.append("\tChar32String utf32() const;")
        result.append("\tCharWideString wide_string() const;")

    if "members" in builtin_api:
        for member in builtin_api["members"]:
            if f'get_{member["name"]}' not in method_list:
                result.append(
                    f'\t{correct_type(member["type"])} get_{member["name"]}() const;'
                )
            if f'set_{member["name"]}' not in method_list:
                result.append(
                    f'\tvoid set_{member["name"]}({type_for_parameter(member["type"])}value);'
                )

    if "operators" in builtin_api:
        for operator in builtin_api["operators"]:
            if operator["name"] not in ["in", "xor"]:
                if "right_type" in operator:
                    result.append(
                        f'\t{correct_type(operator["return_type"])} operator{operator["name"]}({type_for_parameter(operator["right_type"])}other) const;'
                    )
                else:
                    result.append(
                        f'\t{correct_type(operator["return_type"])} operator{operator["name"].replace("unary", "")}() const;'
                    )

    # Special cases.
    if class_name == "String":
        result.append("\toperator const char *() const;")
        result.append("\toperator const char16_t *() const;")
        result.append("\toperator const char32_t *() const;")
        result.append("\toperator const wchar_t *() const;")

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
    result.append(f"#include <variant/{snake_class_name}.hpp>")
    result.append("")
    result.append("#include <godot.hpp>")
    result.append("")

    # Only used since the "fully used" is included in header already.
    for include in used_classes:
        result.append(f"#include <{get_include_path(include)}>")

    if len(used_classes) > 0:
        result.append("")

    result.append("#include <core/builtin_ptrcall.hpp>")
    result.append("")
    result.append("namespace godot {")
    result.append("")

    result.append(f"{class_name}::_MethodBindings {class_name}::_method_bindings;")
    result.append("")

    result.append(f"void {class_name}::init_bindings() {{")

    if "constructors" in builtin_api:
        for constructor in builtin_api["constructors"]:
            result.append(
                f'\t_method_bindings.constructor_{constructor["index"]} = internal::interface->variant_get_ptr_constructor({enum_type_name}, {constructor["index"]});'
            )

    if builtin_api["has_destructor"]:
        result.append(
            f"\t_method_bindings.destructor = internal::interface->variant_get_ptr_destructor({enum_type_name});"
        )

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            # TODO: Add error check for hash mismatch.
            result.append(
                f'\t_method_bindings.method_{method["name"]} = internal::interface->variant_get_ptr_builtin_method({enum_type_name}, "{method["name"]}", {method["hash"]});'
            )

    if "members" in builtin_api:
        for member in builtin_api["members"]:
            result.append(
                f'\t_method_bindings.member_{member["name"]}_setter = internal::interface->variant_get_ptr_setter({enum_type_name}, "{member["name"]}");'
            )
            result.append(
                f'\t_method_bindings.member_{member["name"]}_getter = internal::interface->variant_get_ptr_getter({enum_type_name}, "{member["name"]}");'
            )

    if "indexing_return_type" in builtin_api:
        result.append(
            f"\t_method_bindings.indexed_setter = internal::interface->variant_get_ptr_indexed_setter({enum_type_name});"
        )
        result.append(
            f"\t_method_bindings.indexed_getter = internal::interface->variant_get_ptr_indexed_getter({enum_type_name});"
        )

    if "is_keyed" in builtin_api and builtin_api["is_keyed"]:
        result.append(
            f"\t_method_bindings.keyed_setter = internal::interface->variant_get_ptr_keyed_setter({enum_type_name});"
        )
        result.append(
            f"\t_method_bindings.keyed_getter = internal::interface->variant_get_ptr_keyed_getter({enum_type_name});"
        )
        result.append(
            f"\t_method_bindings.keyed_checker = internal::interface->variant_get_ptr_keyed_checker({enum_type_name});"
        )

    if "operators" in builtin_api:
        for operator in builtin_api["operators"]:
            if "right_type" in operator:
                result.append(
                    f'\t_method_bindings.operator_{get_operator_id_name(operator["name"])}_{operator["right_type"]} = internal::interface->variant_get_ptr_operator_evaluator(GDNATIVE_VARIANT_OP_{get_operator_id_name(operator["name"]).upper()}, {enum_type_name}, GDNATIVE_VARIANT_TYPE_{camel_to_snake(operator["right_type"]).upper()});'
                )
            else:
                result.append(
                    f'\t_method_bindings.operator_{get_operator_id_name(operator["name"])} = internal::interface->variant_get_ptr_operator_evaluator(GDNATIVE_VARIANT_OP_{get_operator_id_name(operator["name"]).upper()}, {enum_type_name}, GDNATIVE_VARIANT_TYPE_NIL);'
                )

    result.append("}")
    result.append("")

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
                method_call += ", &"
                arguments = []
                for argument in constructor["arguments"]:
                    arguments.append(escape_identifier(argument["name"]))
                method_call += ", &".join(arguments)
            method_call += ");"

            result.append(method_call)
            result.append("}")
            result.append("")

    if builtin_api["has_destructor"]:
        result.append(f"{class_name}::~{class_name}() {{")
        result.append("\t_method_bindings.destructor(&opaque);")
        result.append("}")
        result.append("")

    method_list = []

    if "methods" in builtin_api:
        for method in builtin_api["methods"]:
            method_list.append(method["name"])
            method_signature = ""
            if "return_type" in method:
                method_signature += f'{correct_type(method["return_type"])} '
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
                method_call += f'return internal::_call_builtin_method_ptr_ret<{correct_type(method["return_type"])}>('
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

    if "members" in builtin_api:
        for member in builtin_api["members"]:
            if f'get_{member["name"]}' not in method_list:
                result.append(
                    f'{correct_type(member["type"])} {class_name}::get_{member["name"]}() const {{'
                )
                result.append(f'\treturn internal::_call_builtin_ptr_getter<{correct_type(member["type"])}>(_method_bindings.member_{member["name"]}_getter, (GDNativeTypePtr)&opaque);')
                result.append("}")

            if f'set_{member["name"]}' not in method_list:
                result.append(
                    f'void {class_name}::set_{member["name"]}({type_for_parameter(member["type"])}value) {{'
                )
                result.append(f'\t_method_bindings.member_{member["name"]}_setter((GDNativeTypePtr)&opaque, (GDNativeTypePtr)&value);')

                result.append("}")
            result.append("")

    if "operators" in builtin_api:
        for operator in builtin_api["operators"]:
            if operator["name"] not in ["in", "xor"]:
                if "right_type" in operator:
                    result.append(
                        f'{correct_type(operator["return_type"])} {class_name}::operator{operator["name"]}({type_for_parameter(operator["right_type"])}other) const {{'
                    )
                    result.append(f'\treturn internal::_call_builtin_operator_ptr<{correct_type(operator["return_type"])}>(_method_bindings.operator_{get_operator_id_name(operator["name"])}_{operator["right_type"]}, (GDNativeTypePtr)&opaque, (GDNativeTypePtr)&other);')
                    result.append("}")
                else:
                    result.append(
                        f'{correct_type(operator["return_type"])} {class_name}::operator{operator["name"].replace("unary", "")}() const {{'
                    )
                    result.append(f'\treturn internal::_call_builtin_operator_ptr<{correct_type(operator["return_type"])}>(_method_bindings.operator_{get_operator_id_name(operator["name"])}, (GDNativeTypePtr)&opaque, (GDNativeTypePtr)nullptr);')
                    result.append("}")
            result.append("")

    result.append("} //namespace godot")

    return "\n".join(result)


def generate_engine_classes_bindings(api, output_dir, use_template_get_node):
    global engine_classes

    include_gen_folder = Path(output_dir) / "include" / "classes"
    source_gen_folder = Path(output_dir) / "src" / "classes"

    include_gen_folder.mkdir(parents=True, exist_ok=True)
    source_gen_folder.mkdir(parents=True, exist_ok=True)

    # First create map of classes.
    for class_api in api["classes"]:
        engine_classes[class_api["name"]] = class_api["is_refcounted"]

    for class_api in api["classes"]:
        # Check used classes for header include.
        used_classes = set()
        fully_used_classes = set()

        class_name = class_api["name"]

        header_filename = include_gen_folder / (
            camel_to_snake(class_api["name"]) + ".hpp"
        )
        source_filename = source_gen_folder / (
            camel_to_snake(class_api["name"]) + ".cpp"
        )

        if "methods" in class_api:
            for method in class_api["methods"]:
                if "arguments" in method:
                    for argument in method["arguments"]:
                        if is_included(argument["type"], class_name):
                            if is_enum(argument["type"]):
                                fully_used_classes.add(get_enum_class(argument["type"]))
                            elif "default_value" in argument:
                                fully_used_classes.add(argument["type"])
                            else:
                                used_classes.add(argument["type"])
                            if is_refcounted(argument["type"]):
                                fully_used_classes.add("Ref")
                if "return_value" in method:
                    if is_included(method["return_value"]["type"], class_name):
                        if is_enum(method["return_value"]["type"]):
                            fully_used_classes.add(
                                get_enum_class(method["return_value"]["type"])
                            )
                        elif is_variant(method["return_value"]["type"]):
                            fully_used_classes.add(method["return_value"]["type"])
                        else:
                            used_classes.add(method["return_value"]["type"])
                        if is_refcounted(method["return_value"]["type"]):
                            fully_used_classes.add("Ref")

        if "members" in class_api:
            for member in class_api["members"]:
                if is_included(member["type"], class_name):
                    if is_enum(member["type"]):
                        fully_used_classes.add(get_enum_class(member["type"]))
                    else:
                        used_classes.add(member["type"])
                    if is_refcounted(member["type"]):
                        fully_used_classes.add("Ref")

        if "inherits" in class_api:
            if is_included(class_api["inherits"], class_name):
                fully_used_classes.add(class_api["inherits"])
            if is_refcounted(class_api["name"]):
                fully_used_classes.add("Ref")
        else:
            fully_used_classes.add("Wrapped")

        for type_name in fully_used_classes:
            if type_name in used_classes:
                used_classes.remove(type_name)

        with header_filename.open("w+") as header_file:
            header_file.write(
                generate_engine_class_header(
                    class_api, used_classes, fully_used_classes, use_template_get_node
                )
            )

        with source_filename.open("w+") as source_file:
            source_file.write(
                generate_engine_class_source(
                    class_api, used_classes, fully_used_classes, use_template_get_node
                )
            )


def generate_engine_class_header(class_api, used_classes, fully_used_classes, use_template_get_node):
    result = ["// THIS FILE IS GENERATED. EDITS WILL BE LOST."]

    class_name = class_api["name"]
    snake_class_name = camel_to_snake(class_name).upper()

    header_guard = f"GODOT_CPP_{snake_class_name}_HPP"

    result.append(f"#ifndef {header_guard}")
    result.append(f"#define {header_guard}")

    result.append("")

    for included in fully_used_classes:
        result.append(f"#include <{get_include_path(included)}>")

    if len(fully_used_classes) > 0:
        result.append("")

    result.append("namespace godot {")
    result.append("")

    for type_name in used_classes:
        result.append(f"class {type_name};")

    if len(used_classes) > 0:
        result.append("")

    inherits = class_api["inherits"] if "inherits" in class_api else "Wrapped"
    result.append(f"class {class_name} : public {inherits} {{")

    if class_name != "Object":
        result.append(f"\tGDNATIVE_CLASS({class_name}, {inherits})")
        result.append("")

    result.append("public:")

    if "enums" in class_api:
        for enum_api in class_api["enums"]:
            result.append(f'\tenum {enum_api["name"]} {{')
            for value in enum_api["values"]:
                result.append(f'\t\t{value["name"]} = {value["value"]},')
            result.append("\t};")
            result.append("")

    if "methods" in class_api:
        for method in class_api["methods"]:
            if method["is_virtual"]:
                # TODO: See how to bind virtual methods (if they are even needed).
                continue

            method_signature = "\t"
            if "return_value" in method:
                method_signature += correct_type(
                    method["return_value"]["type"],
                    method["return_value"]["meta"]
                    if "meta" in method["return_value"]
                    else None,
                )
            else:
                method_signature += "void"

            if not method_signature.endswith("*"):
                method_signature += " "

            if use_template_get_node and class_name == "Node" and method["name"] == "get_node":
                method_signature += "get_node_internal("
            else:
                method_signature += f'{escape_identifier(method["name"])}('

            if "arguments" in method:
                method_signature += make_function_parameters(
                    method["arguments"], include_default=True
                )

            method_signature += ")"

            if method["is_const"]:
                method_signature += " const"

            method_signature += ";"
            result.append(method_signature)

    # Special cases.
    if class_name == "Object":
        result.append("")
        result.append("\ttemplate<class T>")
        result.append("\tstatic T *cast_to(Object *p_object);")
    elif use_template_get_node and class_name == "Node":
        result.append("\ttemplate<class T>")
        result.append("\tT *get_node(const NodePath &p_path) { return Object::cast_to<T>(get_node_internal(p_path)); }")

    result.append("")
    result.append("};")
    result.append("")
    result.append("} // namespace godot")

    result.append(f"#endif // ! {header_guard}")

    return "\n".join(result)


def generate_engine_class_source(class_api, used_classes, fully_used_classes, use_template_get_node):
    result = ["// THIS FILE IS GENERATED. EDITS WILL BE LOST."]

    class_name = class_api["name"]
    snake_class_name = camel_to_snake(class_name)

    result.append(f"#include <classes/{snake_class_name}.hpp>")
    result.append("")
    result.append(f"#include <core/engine_ptrcall.hpp>")
    result.append(f"#include <core/error_macros.hpp>")
    result.append("")

    for included in used_classes:
        result.append(f"#include <{get_include_path(included)}>")

    if len(used_classes) > 0:
        result.append(f"")

    result.append("namespace godot {")
    result.append("")

    if "methods" in class_api:
        for method in class_api["methods"]:
            if method["is_virtual"]:
                # TODO: See how to bind virtual methods (if they are even needed).
                continue

            # Method signature.
            method_signature = ""
            if "return_value" in method:
                method_signature += correct_type(
                    method["return_value"]["type"],
                    method["return_value"]["meta"]
                    if "meta" in method["return_value"]
                    else None,
                )
            else:
                method_signature += "void"

            if not method_signature.endswith("*"):
                method_signature += " "

            if use_template_get_node and class_name == "Node" and method["name"] == "get_node":
                method_signature += "Node::get_node_internal("
            else:
                method_signature += f'{class_name}::{escape_identifier(method["name"])}('

            if "arguments" in method:
                method_signature += make_function_parameters(
                    method["arguments"], include_default=False
                )

            method_signature += ")"

            if method["is_const"]:
                method_signature += " const"

            method_signature += " {"
            result.append(method_signature)

            # Method body.
            result.append(
                f'\tstatic GDNativeMethodBindPtr ___method_bind = internal::interface->classdb_get_method_bind("{class_name}", "{method["name"]}", {method["hash"]});'
            )
            call = "\t"
            is_ref = False
            if "return_value" in method and method["return_value"]["type"] != "void":
                return_type = method["return_value"]["type"]
                meta_type = (
                    method["return_value"]["meta"]
                    if "meta" in method["return_value"]
                    else None
                )
                result.append(
                    f"\tCHECK_METHOD_BIND_RET(___method_bind, {get_default_value_for_type(return_type)});"
                )
                if (
                    is_pod_type(return_type)
                    or is_variant(return_type)
                    or is_enum(return_type)
                ):
                    call += f"return internal::_call_native_mb_ret<{correct_type(return_type, meta_type)}>(___method_bind, _owner"
                elif is_refcounted(return_type):
                    call += f"return Ref<{return_type}>::___internal_constructor(internal::_call_native_mb_ret_obj(___method_bind, _owner"
                    is_ref = True
                else:
                    call += f"return ({correct_type(return_type)})internal::_call_native_mb_ret_obj(___method_bind, _owner"
            else:
                result.append(f"\tCHECK_METHOD_BIND(___method_bind);")
                call += f"internal::_call_native_mb_no_ret(___method_bind, _owner"

            if "arguments" in method:
                call += ", "
                args = []
                for argument in method["arguments"]:
                    args.append(escape_identifier(argument["name"]))
                call += ", ".join(args)

            if is_ref:
                call += ")"  # Close Ref<> constructor.
            call += ");"
            result.append(call)

            result.append("}")
            result.append("")

    result.append("")
    result.append("} // namespace godot ")

    return "\n".join(result)


def generate_global_constants(api, output_dir):
    include_gen_folder = Path(output_dir) / "include" / "classes"
    source_gen_folder = Path(output_dir) / "src" / "classes"

    include_gen_folder.mkdir(parents=True, exist_ok=True)
    source_gen_folder.mkdir(parents=True, exist_ok=True)

    # Generate header

    header = ["// THIS FILE IS GENERATED. EDITS WILL BE LOST."]
    header_filename = include_gen_folder / "global_constants.hpp"

    header_guard = "GODOT_CPP_GLOBAL_CONSTANTS_HPP"
    header.append(f"#ifndef {header_guard}")
    header.append(f"#define {header_guard}")
    header.append("")
    header.append("namespace godot {")
    header.append("")

    for constant in api["global_constants"]:
        header.append(
            f'\tconst int {escape_identifier(constant["name"])} = {constant["value"]};'
        )

    header.append("")

    for enum_def in api["global_enums"]:
        if enum_def["name"].startswith("Variant."):
            continue

        header.append(f'\tenum {enum_def["name"]} {{')
        for value in enum_def["values"]:
            header.append(f'\t\t{value["name"]} = {value["value"]},')
        header.append("\t};")
        header.append("")

    header.append("} // namespace godot")

    header.append("")
    header.append(f"#endif // ! {header_guard}")

    with header_filename.open("w+") as header_file:
        header_file.write("\n".join(header))


# Helper functions.


def camel_to_snake(name):
    name = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", name)
    name = re.sub("([a-z0-9])([A-Z])", r"\1_\2", name)
    return name.replace("2_D", "2D").replace("3_D", "3D").lower()


def make_function_parameters(parameters, include_default=False):
    signature = []

    for par in parameters:
        parameter = type_for_parameter(
            par["type"], par["meta"] if "meta" in par else None
        )
        parameter += escape_identifier(par["name"])

        if include_default and "default_value" in par:
            parameter += " = "
            if is_enum(par["type"]):
                parameter += f'({correct_type(par["type"])})'
            parameter += correct_default_value(par["default_value"], par["type"])
        signature.append(parameter)

    return ", ".join(signature)


def type_for_parameter(type_name, meta=None):
    if is_pod_type(type_name) and type_name != "Nil" or is_enum(type_name):
        return f"{correct_type(type_name, meta)} "
    elif is_variant(type_name) or is_refcounted(type_name):
        return f"const {correct_type(type_name)} &"
    else:
        return f"{correct_type(type_name)}"


def get_include_path(type_name):
    base_dir = ""
    if type_name == "Object":
        base_dir = "core"
    elif is_variant(type_name):
        base_dir = "variant"
    else:
        base_dir = "classes"

    return f"{base_dir}/{camel_to_snake(type_name)}.hpp"


# Engine idiosyncrasies.


def is_pod_type(type_name):
    """
    Those are types for which no class should be generated.
    """
    return type_name in [
        "Nil",
        "void",
        "int",
        "float",
        "bool",
    ]


def is_enum(type_name):
    return type_name.startswith("enum::")


def get_enum_class(enum_name: str):
    if "." in enum_name:
        return enum_name.replace("enum::", "").split(".")[0]
    else:
        return "GlobalConstants"


def get_enum_name(enum_name: str):
    return enum_name.replace("enum::", "").split(".")[-1]


def is_variant(type_name):
    return type_name == "Variant" or type_name in builtin_classes or type_name == "Nil"


def is_engine_class(type_name):
    return type_name in engine_classes


def is_refcounted(type_name):
    return type_name in engine_classes and engine_classes[type_name]


def is_included(type_name, current_type):
    """
    Check if a builtin type should be included.
    This removes Variant and POD types from inclusion, and the current type.
    """
    to_include = get_enum_class(type_name) if is_enum(type_name) else type_name
    return to_include != current_type and not is_pod_type(to_include)


def correct_default_value(value, type_name):
    value_map = {
        "null": "nullptr",
        '""': "String()",
        '&""': "StringName()",
        "[]": "Array()",
        "{}": "Dictionary()",
        "Transform2D(1, 0, 0, 1, 0, 0)": "Transform2D()",  # Default transform.
        "Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0)": "Transform3D()",  # Default transform.
    }
    if value in value_map:
        return value_map[value]
    if value == "":
        return f"{type_name}()"
    return value


def correct_type(type_name, meta=None):
    type_conversion = {"float": "double", "int": "int64_t", "Nil": "Variant"}
    if meta != None:
        if "int" in meta:
            return f"{meta}_t"
        else:
            return meta
    if type_name in type_conversion:
        return type_conversion[type_name]
    if is_enum(type_name):
        base_class = get_enum_class(type_name)
        if base_class == "GlobalConstants":
            return f"{get_enum_name(type_name)}"
        return f"{base_class}::{get_enum_name(type_name)}"
    if is_refcounted(type_name):
        return f"Ref<{type_name}>"
    if type_name == "Object" or is_engine_class(type_name):
        return f"{type_name} *"
    return type_name


def escape_identifier(id):
    cpp_keywords_map = {
        "class": "_class",
        "char": "_char",
        "short": "_short",
        "bool": "_bool",
        "int": "_int",
        "default": "_default",
        "case": "_case",
        "switch": "_switch",
        "export": "_export",
        "template": "_template",
        "new": "new_",
        "operator": "_operator",
        "typeof": "type_of",
        "typename": "type_name",
    }
    if id in cpp_keywords_map:
        return cpp_keywords_map[id]
    return id


def get_operator_id_name(op):
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
    return op_id_map[op]


def get_default_value_for_type(type_name):
    if type_name == "int":
        return "0"
    if type_name == "float":
        return "0.0"
    if type_name == "bool":
        return "false"
    if is_enum(type_name):
        return f"{correct_type(type_name)}(0)"
    if is_variant(type_name):
        return f"{type_name}()"
    if is_refcounted(type_name):
        return f"Ref<{type_name}>()"
    return "nullptr"


# Just for debugging.
if __name__ == "__main__":
    generate_bindings("godot-headers/extension_api.json", True)

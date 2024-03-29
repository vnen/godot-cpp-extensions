/*************************************************************************/
/*  godot.cpp                                                            */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "godot.hpp"

#include <classes/wrapped.hpp>
#include <core/class_db.hpp>
#include <core/memory.hpp>
#include <variant/variant.hpp>

#include <core/error_macros.hpp>

namespace godot {

namespace internal {

const GDNativeInterface *interface;
GDNativeExtensionClassLibraryPtr library;
void *token;

} // namespace internal

GDNativeBool GDExtensionBinding::init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	internal::interface = p_interface;
	internal::library = p_library;
	internal::token = p_library;

	r_initialization->initialize = initialize_level;
	r_initialization->deinitialize = deinitialize_level;

	Variant::init_bindings();

	return true;
}

void GDExtensionBinding::initialize_level(void *userdata, GDNativeInitializationLevel p_level) {
	ClassDB::initialize(p_level);
}

void GDExtensionBinding::deinitialize_level(void *userdata, GDNativeInitializationLevel p_level) {
	ClassDB::deinitialize(p_level);
}

void *GDExtensionBinding::create_instance_callback(void *p_token, void *p_instance) {
	ERR_FAIL_COND_V_MSG(p_token != internal::library, nullptr, "Asking for creating instance with invalid token.");
	Wrapped *wrapped = memnew(Wrapped(p_instance));
	return wrapped;
}

void GDExtensionBinding::free_instance_callback(void *p_token, void *p_instance, void *p_binding) {
	ERR_FAIL_COND_MSG(p_token != internal::library, "Asking for freeing instance with invalid token.");
	memdelete((Wrapped *)p_binding);
}

} // namespace godot

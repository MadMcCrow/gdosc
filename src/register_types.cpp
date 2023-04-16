/// Copyright (c) 2023 Noé Perard-Gayot (MadMcCrow), François Zajéga (frankiezafe) & Michal Seta (djiamnot)
/// This work is licensed under the terms of the MIT license. For a copy see <https://opensource.org/licenses/MIT>

#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "oscmsg.h"
#include "oscsender.h"
#include "oscreceiver.h"

using namespace godot;

void initialize_gdosc_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

    ClassDB::register_class<osc::oscmsg>();
    ClassDB::register_class<osc::oscsender>();
    ClassDB::register_class<osc::oscreceiver>();
}

void uninitialize_gdosc_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT gdosc_library_init(const GDExtensionInterface *p_interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_initializer(initialize_gdosc_module);
	init_obj.register_terminator(uninitialize_gdosc_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
#include "register_types.h"

#include "circuit.h"
#include "connector.h"
#include "part.h"
#include "scroll_camera.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_cmos_module(ModuleInitializationLevel p_level)
{
    if(p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
    ClassDB::register_class<Part>();
    ClassDB::register_class<ScrollCamera>();
    ClassDB::register_class<Connector>();
    ClassDB::register_class<Circuit>();
}
void uninitialize_cmos_module(ModuleInitializationLevel p_level)
{
    if(p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
}
extern "C" {
GDExtensionBool GDE_EXPORT cmos_library_init(const GDExtensionInterface* p_interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_cmos_module);
    init_obj.register_terminator(uninitialize_cmos_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
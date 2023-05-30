#pragma once

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sstream>

#define BIND_ATR(x, y)                                                                                          \
    do {                                                                                                        \
        godot::ClassDB::bind_method(D_METHOD("get_" #y), &x::get_##y);                                          \
        godot::ClassDB::bind_method(D_METHOD("set_" #y, "p_" #y), &x::set_##y);                                 \
        godot::ClassDB::add_property(#x, godot::PropertyInfo(godot::Variant::FLOAT, #y), "set_" #y, "get_" #y); \
    } while(0)

#define PRT(x) prt(std::stringstream() << x)
inline void prt(std::stringstream str)
{
    godot::UtilityFunctions::print(str.str().c_str());
}
inline void prt(godot::String str)
{
    godot::UtilityFunctions::print(str);
}

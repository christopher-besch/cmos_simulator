#pragma once

#include "cable.h"
#include "helper.h"
#include <vector>

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

enum class PartType : int {
    NONE = 0,
    NMOS = 1,
    PMOS = 2,
};

struct Part: public Node2D {
    GDCLASS(Part, Node2D)

    std::vector<Cable*> cables {};
    PartType            type {PartType::NONE};
    Vector2             size;

protected:
    static void _bind_methods()
    {
        BIND_ATR(Part, type, INT);
        godot::ClassDB::bind_method(D_METHOD("add_cable", "cable"), &Part::add_cable);
    }

public:
    int get_type() const
    {
        return static_cast<int>(type);
    }
    void set_type(int p_type)
    {
        type = static_cast<PartType>(p_type);
    }
    void add_cable(Cable* cable)
    {
        cables.push_back(cable);
        cable->for_part = this;
    }
};

} // namespace godot

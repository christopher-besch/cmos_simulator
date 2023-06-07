#pragma once

#include "cable.h"
#include "helper.h"
#include <vector>

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

enum class PartType : int {
    NONE = 0,
    NMOS = 1,
    PMOS = 2,
};

struct Part: public Node2D {
    GDCLASS(Part, Node2D)

    PartType type {PartType::NONE};
    Vector2  size;

    // special handling here
private:
    std::vector<Cable*> cables {};

protected:
    static void _bind_methods()
    {
        BIND_ATR(Part, type, INT);
    }

public:
    // cables can't be changed after this is called
    std::vector<Cable*>& get_cables()
    {
        if(cables.size())
            return cables;
        TypedArray<Node> children = get_children();
        for(int i {0}; i < children.size(); ++i) {
            Cable* child = Object::cast_to<Cable>(children[i]);
            if(!child)
                continue;
            cables.push_back(child);
            child->for_part = this;
        }
        return cables;
    }

    int get_type() const
    {
        return static_cast<int>(type);
    }
    void set_type(int p_type)
    {
        type = static_cast<PartType>(p_type);
    }
};

} // namespace godot

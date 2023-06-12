#pragma once

#include "cable.h"
#include "helper.h"
#include <map>
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

    // load_cables needs to be called first
    std::vector<Cable*>             cables;
    std::map<ConnectorType, Cable*> start_cables;
    int                             id;

protected:
    static void _bind_methods()
    {
        BIND_ATR(Part, type, INT);
    }

public:
    // cables can't be changed after this is called
    void load_cables()
    {
        cables.resize(0);
        TypedArray<Node> children = get_children();
        for(int i {0}; i < children.size(); ++i) {
            Cable* child = Object::cast_to<Cable>(children[i]);
            if(!child)
                continue;
            cables.push_back(child);
            child->for_part = this;

            if(child->type != ConnectorType::NONE)
                start_cables[child->type] = child;
        }
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

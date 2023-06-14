#pragma once

#include "cable.h"
#include "helper.h"

#include <map>
#include <vector>

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

enum class PartType : int {
    NONE  = 0,
    NMOS  = 1,
    PMOS  = 2,
    GND   = 3,
    LABEL = 4,
};

struct Part: public Node2D {
    GDCLASS(Part, Node2D)

    PartType type {PartType::NONE};
    Vector2  size;

    // load_cables needs to be called first
    std::vector<Cable*>             cables;
    std::vector<Node2D*>            no_rotate_nodes;
    std::map<ConnectorType, Cable*> start_cables;
    int                             id;

protected:
    static void _bind_methods()
    {
        BIND_ATR(Part, type, INT);
    }

public:
    // cables and labels can't be changed after this is called
    void load()
    {
        cables.resize(0);
        no_rotate_nodes.resize(0);
        TypedArray<Node> children = get_children();
        for(int i {0}; i < children.size(); ++i) {
            Cable* cable = Object::cast_to<Cable>(children[i]);
            if(cable) {
                cables.push_back(cable);
                cable->for_part = this;

                if(cable->type != ConnectorType::NONE)
                    start_cables[cable->type] = cable;
            }
        }

        Node2D* non_rotate_parent = get_node<Node2D>("NonRotate");
        if(!non_rotate_parent) {
            // TODO: fuckup
            PRT("dafuq");
            return;
        }
        TypedArray<Node> non_rotate_node = non_rotate_parent->get_children();
        for(int i {0}; i < non_rotate_node.size(); ++i) {
            Node2D* node = Object::cast_to<Node2D>(non_rotate_node[i]);
            if(!node) {
                // TODO: fuckup
                PRT("dafuq");
                continue;
            }
            no_rotate_nodes.push_back(node);
        }

        size = get_node<Sprite2D>("Sprite")->get_texture()->get_size();
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

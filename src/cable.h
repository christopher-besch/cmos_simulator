#pragma once

#include "helper.h"

#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/segment_shape2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <vector>

namespace godot {

struct Part;

enum class ConnectorType : int {
    NONE = 0,
    // nmos & pmos
    Source = 1,
    Drain  = 2,
    Gate   = 3,
    Bulk   = 4,

    GND = 5,
    // used for label
    CABLE = 6,
};

struct Cable: public Line2D {
    GDCLASS(Cable, Line2D)

    ConnectorType type {ConnectorType::NONE};
    Part*         for_part {nullptr};

protected:
    static void _bind_methods()
    {
        BIND_ATR(Cable, type, INT);
    }

public:
    int get_type() const
    {
        return static_cast<int>(type);
    }
    void set_type(int p_type)
    {
        type = static_cast<ConnectorType>(p_type);
    }

    Vector2i get_global_a() const
    {
        return to_global(get_point_position(0)).round();
    }
    Vector2i get_global_b() const
    {
        return to_global(get_point_position(1)).round();
    }
    void set_a(Vector2i pos)
    {
        SegmentShape2D* shape = Object::cast_to<SegmentShape2D>(get_node<CollisionShape2D>("Area2D/CollisionShape2D")->get_shape().ptr());
        shape->set_a(pos);
        set_point_position(0, to_local(pos));
    }
    void set_b(Vector2i pos)
    {
        SegmentShape2D* shape = Object::cast_to<SegmentShape2D>(get_node<CollisionShape2D>("Area2D/CollisionShape2D")->get_shape().ptr());
        shape->set_b(pos);
        set_point_position(1, to_local(pos));
    }
    void set_global_a(Vector2i pos)
    {
        set_a(to_local(pos));
    }
    void set_global_b(Vector2i pos)
    {
        set_b(to_local(pos));
    }
};

} // namespace godot

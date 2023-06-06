#pragma once

#include "helper.h"

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/line2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
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
};

} // namespace godot
